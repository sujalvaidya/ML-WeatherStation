const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Weather Station Dashboard</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet"
          integrity="sha384-QWTKZyjpPEjISv5WaRU9OFeRpok6YctnYmDr5pNlyT2bRjXh0JMhjY6hW+ALEwIH" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        .clock {
            height: 150px;
            width: 150px;
            position: relative;
            margin: 0 auto;
        }

        .hour, .min {
            background: #f5f5f5;
            position: absolute;
            left: 50%;
            transform-origin: bottom center;
            transition: all .5s linear;
            border-radius: 5px;
        }

        .hour {
            height: 50px;
            width: 4px;
            top: 25px;
        }

        .min {
            height: 65px;
            width: 4px;
            top: 10px;
        }

        .info-title {
            font-size: 14px;
            margin-bottom: 5px;
        }

        .info-value {
            font-size: 30px;
        }

        .chart-container {
            background-color: #fff;
            border-radius: 10px;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
            padding: 20px;
            margin: 10px 0;
        }

        .card-header {
            background-color: #00A8A9;
            color: white;
            font-size: 18px;
            padding: 10px;
        }

        .card-body {
            padding: 20px;
        }

        .widget {
            background-color: #f8f9fa;
            border-radius: 10px;
            padding: 20px;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        }

        .widget h1 {
            font-size: 3.2rem;
            color: #333;
        }

        .widget h2 {
            font-size: 2rem;
            color: #333;
        }

        .widget h3 {
            font-size: 3rem;
            color: #00A8A9;
        }

        .infos {
            background-color: #00A8A9;
            border-radius: 10px;
            padding: 20px;
            color: white;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        }

        .infos .info-title {
            font-size: 1rem;
        }

        .infos .info-value {
            font-size: 1.5rem;
        }
    </style>
</head>
<body>
<div class="container">
    <div class="widget row align-items-center rounded-4 bg-light my-3 py-3">
        <div class="col-12 text-center col-sm-4 my-3">
            <h1>Weather Station</h1>
        </div>
        <div class="col-6 text-center  col-sm-4">
            <h2 class="date" id="date"></h2>
            <!--            <h6>Temperature</h6>-->
            <h3 class="place update" id="temperature">23&deg;C</h3>
        </div>
        <div class="col-6 col-sm-4">
            <div class="row justify-content-center">
                <div class="clock rounded-circle" style="background-color: #00A8A9;">
                    <div class="min" id="min"></div>
                    <div class="hour" id="hour"></div>
                </div>
            </div>
        </div>
    </div>

    <div class="infos row justify-content-evenly text-center fw-light text-white rounded-4 my-3 py-3">
        <div class="info col-6 col-sm-2 my-2">
            <div class="info-title">PRESSURE</div>
            <div class="info-value update" id="pressure">0 pa</div>
        </div>
        <div class="info col-6 col-sm-2 my-2">
            <div class="info-title">RAIN</div>
            <div class="info-value update" id="rain">0mm</div>
        </div>
        <div class="info col-6 col-sm-2 my-2">
            <div class="info-title">HUMIDITY</div>
            <div class="info-value update" id="humidity">23</div>
        </div>
        <div class="info col-6 col-sm-2 my-2">
            <div class="info-title">WIND SPEED</div>
            <div class="info-value update" id="wind-speed">54</div>
        </div>
        <div class="info col-6 col-sm-2 my-2">
            <div class="info-title">PREDICTED TEMP.</div>
            <div class="info-value update" id="predicted-temp">23</div>
        </div>
    </div>

    <div class="row justify-content-center" id="charts">
        <div class="chart-container col-sm-12 col-md-6 col-lg-4">
            <div class="card">
                <div class="card-header">
                    Temperature Over Time
                </div>
                <div class="card-body">
                    <canvas id="temperatureChart"></canvas>
                    <!--                    <p class="text-muted">Displays current and past temperatures for the last 24 hours.</p>-->
                </div>
            </div>
        </div>
        <div class="chart-container col-sm-12 col-md-6 col-lg-4">
            <div class="card">
                <div class="card-header">
                    Pressure Over Time
                </div>
                <div class="card-body">
                    <canvas id="pressureChart"></canvas>
                    <!--                    <p class="text-muted">Displays pressure readings for the last 24 hours.</p>-->
                </div>
            </div>
        </div>
        <div class="chart-container col-sm-12 col-md-6 col-lg-4">
            <div class="card">
                <div class="card-header">
                    Rainfall Over Time
                </div>
                <div class="card-body">
                    <canvas id="rainChart"></canvas>
                    <!--                    <p class="text-muted">Displays rainfall data for the last 24 hours.</p>-->
                </div>
            </div>
        </div>
        <div class="chart-container col-sm-12 col-md-6 col-lg-4">
            <div class="card">
                <div class="card-header">
                    Humidity Over Time
                </div>
                <div class="card-body">
                    <canvas id="humidityChart"></canvas>
                    <!--                    <p class="text-muted">Displays humidity levels for the last 24 hours.</p>-->
                </div>
            </div>
        </div>
        <div class="chart-container col-sm-12 col-md-6 col-lg-4">
            <div class="card">
                <div class="card-header">
                    Wind Speed Over Time
                </div>
                <div class="card-body">
                    <canvas id="windSpeedChart"></canvas>
                    <!--                    <p class="text-muted">Displays wind speed readings for the last 24 hours.</p>-->
                </div>
            </div>
        </div>
        <div class="chart-container col-sm-12 col-md-6 col-lg-4">
            <div class="card">
                <div class="card-header">
                    Predicted Temperature
                </div>
                <div class="card-body">
                    <canvas id="predictedTemperatureChart"></canvas>
                    <!--                    <p class="text-muted">Displays predicted temperatures for the next 24 hours.</p>-->
                </div>
            </div>
        </div>
    </div>
</div>

<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/bootstrap.bundle.min.js"
        integrity="sha384-YvpcrYf0tY3lHB60NNkmXc5s9fDVZLESaAA55NDzOxhy9GkcIdslK1eN7N6jIeHz"
        crossorigin="anonymous"></script>
<script>
    setInterval(drawClock, 2000);

    function drawClock() {
        let now = new Date();
        let hour = now.getHours();
        let minute = now.getMinutes();

        let options = {year: 'numeric', month: 'long', day: 'numeric'};
        let today = new Date();
        document.getElementById("date").innerHTML = today.toLocaleDateString("en-US", options);

        let hourAngle = (360 * (hour / 12)) + ((360 / 12) * (minute / 60));
        let minAngle = 360 * (minute / 60);
        document.getElementById("hour").style.transform = "rotate(" + (hourAngle) + "deg)";
        document.getElementById("min").style.transform = "rotate(" + (minAngle) + "deg)";
    }

</script>
<script>

    setInterval(updateData, 2000);

    let temperatureChart, pressureChart, rainChart, humidityChart, windSpeedChart, predictedTemperatureChart;

    function updateData() {
        let xhr = new XMLHttpRequest();
        xhr.onload = function () {
            if (xhr.status === 200) {
                let data = JSON.parse(xhr.responseText);
                console.log(data);
                updateUI(data);
            }
        };
        xhr.open('GET', 'getData', true);
        // xhr.open('GET', 'http://192.168.48.177/getData', true);
        xhr.send();
    }

    function updateUI(data) {
        document.getElementById('temperature').innerHTML = data.currentTemperature + '&deg;C';
        document.getElementById('rain').innerHTML = data.currentRain + ' mm';
        document.getElementById('wind-speed').innerHTML = data.currentWindSpeed + ' m/s';
        document.getElementById('humidity').innerHTML = data.currentHumidity;
        document.getElementById('pressure').innerHTML = data.currentPressure + ' pa';
        document.getElementById('predicted-temp').innerHTML = data.currentPredictedTemperature + '&deg;C';

        // Update charts with new data
        updateCharts(data);
    }

    function updateCharts(data) {
        updateTemperatureChart(data);
        updatePressureChart(data);
        updateRainChart(data);
        updateHumidityChart(data);
        updateWindSpeedChart(data);
        updatePredictedTemperatureChart(data);
    }

    function updateTemperatureChart(data) {
        if (!temperatureChart) {
            const temperatureCtx = document.getElementById('temperatureChart').getContext('2d');
            temperatureChart = new Chart(temperatureCtx, {
                type: 'line',
                data: {
                    labels: data.hours,
                    datasets: [{
                        label: 'Temperature',
                        data: data.temperatureData24,
                        borderColor: 'rgba(255, 99, 132, 1)',
                        borderWidth: 1,
                        fill: false
                    },
                        {
                            label: 'Predicted Temperature',
                            data: data.pastTemperatureData24,
                            borderColor: 'rgba(54, 162, 235, 1)',
                            borderWidth: 1,
                            fill: false
                        }]
                }
            });
        } else {
            temperatureChart.data.labels = data.hours;
            temperatureChart.data.datasets[0].data = data.temperatureData24;
            temperatureChart.data.datasets[1].data = data.pastTemperatureData24;
            temperatureChart.update();
        }
    }

    function updatePressureChart(data) {
        if (!pressureChart) {
            const pressureCtx = document.getElementById('pressureChart').getContext('2d');
            pressureChart = new Chart(pressureCtx, {
                type: 'line',
                data: {
                    labels: data.hours,
                    datasets: [{
                        label: 'Pressure',
                        data: data.pressureData24,
                        borderColor: 'rgba(75, 192, 192, 1)',
                        borderWidth: 1,
                        fill: false
                    }]
                }
            });
        } else {
            pressureChart.data.labels = data.hours;
            pressureChart.data.datasets[0].data = data.pressureData24;
            pressureChart.update();
        }
    }

    function updateRainChart(data) {
        if (!rainChart) {
            const rainCtx = document.getElementById('rainChart').getContext('2d');
            rainChart = new Chart(rainCtx, {
                type: 'line',
                data: {
                    labels: data.hours,
                    datasets: [{
                        label: 'Rain',
                        data: data.rainData24,
                        borderColor: 'rgba(153, 102, 255, 1)',
                        borderWidth: 1,
                        fill: false
                    }]
                }
            });
        } else {
            rainChart.data.labels = data.hours;
            rainChart.data.datasets[0].data = data.rainData24;
            rainChart.update();
        }
    }

    function updateHumidityChart(data) {
        if (!humidityChart) {
            const humidityCtx = document.getElementById('humidityChart').getContext('2d');
            humidityChart = new Chart(humidityCtx, {
                type: 'line',
                data: {
                    labels: data.hours,
                    datasets: [{
                        label: 'Humidity',
                        data: data.humidityData24,
                        borderColor: 'rgba(255, 159, 64, 1)',
                        borderWidth: 1,
                        fill: false
                    }]
                }
            });
        } else {
            humidityChart.data.labels = data.hours;
            humidityChart.data.datasets[0].data = data.humidityData24;
            humidityChart.update();
        }
    }

    function updateWindSpeedChart(data) {
        if (!windSpeedChart) {
            const windSpeedCtx = document.getElementById('windSpeedChart').getContext('2d');
            windSpeedChart = new Chart(windSpeedCtx, {
                type: 'line',
                data: {
                    labels: data.hours,
                    datasets: [{
                        label: 'Wind Speed',
                        data: data.windSpeedData24,
                        borderColor: 'rgba(255, 206, 86, 1)',
                        borderWidth: 1,
                        fill: false
                    }]
                }
            });
        } else {
            windSpeedChart.data.labels = data.hours;
            windSpeedChart.data.datasets[0].data = data.windSpeedData24;
            windSpeedChart.update();
        }
    }

    function updatePredictedTemperatureChart(data) {
        if (!predictedTemperatureChart) {
            const predictedTemperatureCtx = document.getElementById('predictedTemperatureChart').getContext('2d');
            predictedTemperatureChart = new Chart(predictedTemperatureCtx, {
                type: 'line',
                data: {
                    labels: data.hours,
                    datasets: [{
                        label: 'Predicted Temperature',
                        data: data.predictedTemperatureData24,
                        borderColor: 'rgba(75, 192, 192, 1)',
                        borderWidth: 1,
                        fill: false
                    }]
                }
            });
        } else {
            predictedTemperatureChart.data.labels = data.hours;
            predictedTemperatureChart.data.datasets[0].data = data.predictedTemperatureData24;
            predictedTemperatureChart.update();
        }
    }


    // document.addEventListener('DOMContentLoaded', function () {
    //     updateData();
    // });

</script>
</body>
</html>

)=====";