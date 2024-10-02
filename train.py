import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn.ensemble import RandomForestRegressor
from sklearn.metrics import mean_squared_error, mean_absolute_error, r2_score, explained_variance_score
import joblib


csv_file_path = 'bangalore.csv'  # Update this with your file path
data = pd.read_csv(csv_file_path)


data = data.drop(columns=['predicted_temperature_c'])


data['datetime_utc'] = pd.to_datetime(data['datetime_utc'], format='%d-%m-%Y %H:%M')


data = data.dropna()

for lag in range(1, 25):
    data[f'temperature_lag_{lag}'] = data['temperature_C'].shift(lag)


data = data.dropna()


data['hour'] = data['datetime_utc'].dt.hour
data['hour_sin'] = np.sin(2 * np.pi * data['hour'] / 24)
data['hour_cos'] = np.cos(2 * np.pi * data['hour'] / 24)

data['month'] = data['datetime_utc'].dt.month
data['month_sin'] = np.sin(2 * np.pi * data['month'] / 12)
data['month_cos'] = np.cos(2 * np.pi * data['month'] / 12)

data['day_of_year'] = data['datetime_utc'].dt.dayofyear
data['day_of_year_sin'] = np.sin(2 * np.pi * data['day_of_year'] / 365.25)
data['day_of_year_cos'] = np.cos(2 * np.pi * data['day_of_year'] / 365.25)


X = data.drop(['temperature_C', 'datetime_utc', 'coordinates', 'utc_offset_h'], axis=1)
y = data['temperature_C']


X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

scaler = StandardScaler()


X_train_scaled = scaler.fit_transform(X_train)


X_test_scaled = scaler.transform(X_test)


best_params = {'max_depth': 20, 'min_samples_leaf': 1, 'min_samples_split': 2, 'n_estimators': 300}
best_rf_model = RandomForestRegressor(**best_params, random_state=42)


best_rf_model.fit(X_train_scaled, y_train)


joblib.dump(best_rf_model, 'best_rf_model.pkl')
print("Best model saved as 'best_rf_model.pkl'.")


joblib.dump(scaler, 'scaler.pkl')
print("Scaler saved as 'scaler.pkl'.")


y_pred = best_rf_model.predict(X_test_scaled)


mse = mean_squared_error(y_test, y_pred)
mae = mean_absolute_error(y_test, y_pred)
r2 = r2_score(y_test, y_pred)
variance_score = explained_variance_score(y_test, y_pred)

print(f'Mean Squared Error: {mse}')
print(f'Mean Absolute Error: {mae}')
print(f'R^2 Score: {r2}')
print(f'Variance Score: {variance_score}')


predictions_df = pd.DataFrame({'Actual': y_test, 'Predicted': y_pred})
print(predictions_df.head())
