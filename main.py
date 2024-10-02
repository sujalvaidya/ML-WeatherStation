import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestRegressor
from datetime import datetime, timedelta, timezone
import requests
import requests_cache
from openmeteo_requests import Client
import json
from retry_requests import retry
import threading


def read_weather_csv():
    return pd.read_csv('bangalore.csv', parse_dates=['datetime_utc'], index_col='datetime_utc', dayfirst=True)


def predict_temperature():
    weather_df = read_weather_csv()
    weather_df.index = pd.to_datetime(weather_df.index, format='mixed')


    necessary_columns = ['temperature_C', 'relative_humidity', 'wind_speed_mps', 'total_precipitation_mm',
                         'surface_pressure_pa']
    weather_df_num = weather_df.loc[:, necessary_columns].dropna()

    weather_y = weather_df_num.pop("temperature_C")
    weather_x = weather_df_num

    train_X, test_X, train_y, test_y = train_test_split(weather_x, weather_y, test_size=0.2, random_state=4)
    last_day_data = weather_df_num.iloc[-24:]

    regr = RandomForestRegressor(max_depth=90, random_state=0, n_estimators=100)
    regr.fit(train_X, train_y)

    hourly_prediction = regr.predict(last_day_data)
    next_hour_predicted_temp = hourly_prediction.tolist()[0]  # Average temperature for the next day

    return next_hour_predicted_temp, hourly_prediction.tolist()


def get_wind_speed():
    cache_session = requests_cache.CachedSession('.cache', expire_after=3600)
    retry_session = retry(cache_session, retries=5, backoff_factor=0.2)
    openmeteo = Client(session=retry_session)

    url = "https://api.open-meteo.com/v1/forecast"
    params = {
        "latitude": 12.9351,
        "longitude": 77.536,
        "current": "wind_speed_10m",
        "wind_speed_unit": "ms",
        "timezone": "auto",
        "forecast_days": 1
    }
    responses = openmeteo.weather_api(url, params=params)
    response = responses[0]
    current = response.Current()
    current_wind_speed_10m = current.Variables(0).Value()

    return current_wind_speed_10m


def update_csv():
    url = 'http://192.168.67.177/getData'
    headers = {'Content-Type': 'application/json'}
    response = requests.get(url, headers=headers)

    if response.status_code == 200:
        data = response.json()

        current_temperature_C = data["currentTemperature"]
        current_relative_humidity = data["currentHumidity"]
        current_wind_speed_mps = data["currentWindSpeed"]
        current_total_precipitation_mm = data["currentRain"]
        current_surface_pressure_pa = data["currentPressure"]
        current_predicted_temperature_c = data["currentPredictedTemperature"]

        current_time = datetime.now(timezone.utc).replace(minute=0, second=0, microsecond=0)
        formatted_time = current_time.strftime("%d-%m-%Y %H:%M:%S")

        new_data = {
            'datetime_utc': formatted_time,
            'coordinates': '(12.934192, 77.5348405)',
            'utc_offset_h': 5.5,
            'temperature_C': current_temperature_C,
            'relative_humidity': current_relative_humidity,
            'wind_speed_mps': current_wind_speed_mps,
            'total_precipitation_mm': current_total_precipitation_mm,
            'surface_pressure_pa': current_surface_pressure_pa,
            'predicted_temperature_c': current_predicted_temperature_c
        }

        df = pd.read_csv('bangalore.csv')
        necessary_columns = ['temperature_C', 'relative_humidity', 'wind_speed_mps', 'total_precipitation_mm',
                             'surface_pressure_pa']
        df = df.dropna(subset=necessary_columns)


        df['datetime_utc'] = pd.to_datetime(df['datetime_utc'], format='mixed')


        if current_time in df['datetime_utc'].values:
            # Overwrite the existing row
            df.loc[df['datetime_utc'] == current_time, :] = pd.DataFrame(new_data, index=[0])
        else:
            # Append the new data
            df = df._append(new_data, ignore_index=True)


        df.to_csv('bangalore.csv', index=False)
        print(f"Updated csv with {new_data}")
    else:
        print(f"Failed to retrieve data: {response.status_code}")


def send_data():
    weather_df = read_weather_csv()
    weather_df.index = pd.to_datetime(weather_df.index, format='mixed')


    necessary_columns = ['temperature_C', 'relative_humidity', 'wind_speed_mps', 'total_precipitation_mm', 'surface_pressure_pa']
    weather_df = weather_df.dropna(subset=necessary_columns)

    past_24_hours = weather_df.iloc[-24:]
    past_24_hours.index = past_24_hours.index + timedelta(hours=5.5)
    hours = past_24_hours.index.strftime('%H:%M').tolist()
    temperature_data = past_24_hours['temperature_C'].tolist()
    past_temperature_data = past_24_hours['predicted_temperature_c'].tolist()
    pressure_data = past_24_hours['surface_pressure_pa'].tolist()
    rain_data = past_24_hours['total_precipitation_mm'].tolist()
    humidity_data = past_24_hours['relative_humidity'].tolist()
    wind_speed_data = past_24_hours['wind_speed_mps'].tolist()

    current_predicted_temperature, predicted_temperature_data = predict_temperature()
    current_wind_speed = get_wind_speed()

    data = {
        "hours": hours,
        "temperatureData24": temperature_data,
        "pressureData24": pressure_data,
        "rainData24": rain_data,
        "humidityData24": humidity_data,
        "windSpeedData24": wind_speed_data,
        "predictedTemperatureData24": predicted_temperature_data,
        "currentPredictedTemperature": current_predicted_temperature,
        "currentWindSpeed": current_wind_speed,
        "pastTemperatureData24": past_temperature_data,
    }

    print(data)

    url = 'http://192.168.67.177/updateData'
    headers = {'Content-Type': 'application/json'}
    response = requests.post(url, headers=headers, data=json.dumps(data))
    if response.status_code == 200:
        print("Sent data to nodemcu successfully")
    else:
        print("Failed to send data to nodemcu")


def run_periodically(interval, function):
    def wrapper():
        function()
        threading.Timer(interval, wrapper).start()

    wrapper()


if __name__ == "__main__":
    run_periodically(1200, send_data)  # Run every 20 minutes
    run_periodically(1200, update_csv)  # Run every 20 minutes



