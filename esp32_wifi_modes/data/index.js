// handle environment ***********************************************************************************************

const displayTemperatureElement = document.getElementById("display-temperature");
const displayHumidityElement = document.getElementById("display-humidity");
const chartHumidityElement = document.getElementById("chart-humidity");


const chartHumidity = new Chart(chartHumidityElement, {
    type: 'line',
    data: {
        labels: [
            '00:00:01', '00:00:02', '00:00:03', '00:00:04', '00:00:05', '00:00:06', '00:00:07', '00:00:08', '00:00:09', '00:00:10', 
            '00:00:11', '00:00:12', '00:00:13', '00:00:14', '00:00:15', '00:00:16', '00:00:17', '00:00:18', '00:00:19', '00:00:20', 
            '00:00:21', '00:00:22', '00:00:23', '00:00:24', '00:00:25', '00:00:26', '00:00:27', '00:00:28', '00:00:29', '00:00:30', 
            '00:00:31', '00:00:32', '00:00:33', '00:00:34', '00:00:35', '00:00:36', '00:00:37', '00:00:38', '00:00:39', '00:00:40'
        ],
        datasets: [
            {
                label: '*C',
                data: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                borderWidth: 1,
                borderColor: "red",
                tension: 0.4,
                pointRadius: 0,
                borderWidth: 1.5,
            }, {
                label: '%RH',
                data: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                borderWidth: 1,
                borderColor: "blue",
                tension: 0.4,
                pointRadius: 0,
                borderWidth: 1.5,
            }
        ]
    },
    options: {
        scales: {
            y: {
                min: 0
            }
        },
        animation: false,
        parse: false,
        normalized: true
    }
});

function shiftChartData(chart, label, newData) {
    chart.data.labels.shift();
    chart.data.labels.push(label);
    for(let i = 0; i < chart.data.datasets.length; i++) {
        chart.data.datasets[i].data.shift();
        chart.data.datasets[i].data.push(newData[i]);
    }
    chart.update('none');
}

var temperature = 0;
var humidity = 0;

setInterval(() => {
    const d = new Date();
    let time = "" + d.getHours() +":"+ d.getMinutes() +":"+ d.getSeconds();

    displayTemperatureElement.textContent = temperature;
    displayHumidityElement.textContent = humidity;

    shiftChartData(chartHumidity, time, [temperature, humidity]);
}, 15000);

// handle light ******************************************************************************************

const bedRoomElement = document.getElementById("bed-room");
const livingRoomElement = document.getElementById("living-room");

bedRoomElement.addEventListener("click", toggleBedRoomLight);
livingRoomElement.addEventListener("click", toggleLivingRoomLight);

var isBedRoomLightOn = true;
var isLivingRoomLightOn = true;

function displayBedRoomLight(data) {
    console.log("Bed room light: " + data);
    isBedRoomLightOn = data;
    bedRoomElement.style.backgroundColor = isBedRoomLightOn ? "yellow" : "grey";
}

function displayLivingRoomLight(data) {
    console.log("Living room light: " + data);
    isLivingRoomLightOn = data;
    livingRoomElement.style.backgroundColor = isLivingRoomLightOn ? "yellow" : "grey";
}

function toggleBedRoomLight() {
    isBedRoomLightOn = !isBedRoomLightOn;
    socket.send(`bedRoomLight${isBedRoomLightOn === true ? 1 : 0}`);
}

function toggleLivingRoomLight() {
    isLivingRoomLightOn = !isLivingRoomLightOn;
    socket.send(`livingRoomLight${isLivingRoomLightOn === true ? 1 : 0}`);
}

function setBedRoomLight(state) {
    isBedRoomLightOn = state;
    socket.send(`bedRoomLight${isBedRoomLightOn === true ? 1 : 0}`);
}

function setLivingRoomLight(state) {
    isLivingRoomLightOn = state;
    socket.send(`livingRoomLight${isLivingRoomLightOn === true ? 1 : 0}`);
}

// schedule *****************************************************************************************

const bedRoomSwitchOnlyButton = document.getElementById("bed-room-switch-only-btn");
const bedRoomAutomaticButton = document.getElementById("bed-room-automatic-btn");
const bedRoomScheduledButton = document.getElementById("bed-room-scheduled-btn");
const bedRoomScheduleElement = document.getElementById("bed-room-schedule");
const bedRoomScheduleButton = document.getElementById("bed-room-schedule-btn");
const addTimeButton = document.getElementById("add-time-btn");
const popTimeButton = document.getElementById("pop-time-btn");
const inputTimeHours = document.getElementById("input-time-hours");
const inputTimeMinutes = document.getElementById("input-time-minutes");
const inputTimeSeconds = document.getElementById("input-time-seconds");
const scheduleErrorMessageElement = document.getElementById("schedule-error-message");
const scheduleSection = document.getElementById("schedule-section");


bedRoomSwitchOnlyButton.addEventListener("click", () => {setBedRoomLightingMode(0)});
bedRoomAutomaticButton.addEventListener("click", () => {setBedRoomLightingMode(1)});
bedRoomScheduledButton.addEventListener("click", () => {setBedRoomLightingMode(2)});
addTimeButton.addEventListener("click", addTime);
popTimeButton.addEventListener("click", popTime);
bedRoomScheduleButton.addEventListener("click", setBedRoomSchedule);

var bedRoomLightSchedule = [];
var bedRoomLightingMode = 0;

function displayBedRoomLightingMode() {
    const buttons = [bedRoomSwitchOnlyButton, bedRoomAutomaticButton, bedRoomScheduledButton];

    for(var i = 0; i < buttons.length; i++) {
        buttons[i].style.backgroundColor = "black";
        buttons[i].style.color = "white";
        buttons[i].style.boxShadow = "0 0 0 0";
    }

    buttons[bedRoomLightingMode].style.backgroundColor = "white";
    buttons[bedRoomLightingMode].style.color = "black";
    buttons[bedRoomLightingMode].style.boxShadow = "0px 0px 5px 2px black";

    if(bedRoomLightingMode == 2) {
        scheduleSection.style.display = "contents";
    }
    else {
        scheduleSection.style.display = "none";
    }
}


function displayBedRoomSchedule() {
    console.log("Bed room light schedule: " + bedRoomLightSchedule);
    let scheduleString = "";
    for (let i = 0; i < bedRoomLightSchedule.length; i++) {
        let hour = parseInt(bedRoomLightSchedule[i] / 3600);
        let minute = parseInt((bedRoomLightSchedule[i] - (hour * 3600)) / 60);
        let second = parseInt(bedRoomLightSchedule[i] - (hour * 3600) - (minute * 60));
        let status = (i%2==0? "on" : "off")
        scheduleString += `${hour}:${minute}:${second} --(${status})-- `;
    }
    bedRoomScheduleElement.textContent = scheduleString;
}

function setBedRoomLightingMode(mode) {
    bedRoomLightingMode = mode;
    console.log("bed room light mode: " + bedRoomLightingMode);
    socket.send(`bedRoomLightingMode${bedRoomLightingMode}`);
}

function checkInputSchedule(element) {
    if(element.value === "") return 0;
    if(isNaN(parseInt(element.value))) return -1;
    else return parseInt(element.value);
}

function addTime() {
    const hoursValue = checkInputSchedule(inputTimeHours);
    const minutesValue = checkInputSchedule(inputTimeMinutes);
    const secondsValue = checkInputSchedule(inputTimeSeconds);

    if(hoursValue === -1 || minutesValue === -1 || secondsValue === -1) {
        scheduleErrorMessageElement.textContent = "Please enter valid time";
    }
    else {
        let time = 0;
        time += hoursValue * 3600;
        time += minutesValue * 60;
        time += secondsValue;
        
        if(!bedRoomLightSchedule.includes(time)) {
            bedRoomLightSchedule.push(time);
            bedRoomLightSchedule.sort(function(a, b){return a-b});
            displayBedRoomSchedule();
        }
    
        scheduleErrorMessageElement.textContent = "";
    }
}

function popTime() {
    bedRoomLightSchedule.pop();
    displayBedRoomSchedule();
}

function setBedRoomSchedule() {
    socket.send(`bedRoomLightSchedule${bedRoomLightSchedule.join(",")}`);
    scheduleErrorMessageElement.textContent = "scheduled!";
    setTimeout(function() {
        scheduleErrorMessageElement.textContent = "";
    }, 2000);
}


// Speech ****************************************************************

const micBtn = document.querySelector('.talk');
const content = document.querySelector('.content');

function speak(sentence) {
    const speech = new SpeechSynthesisUtterance(sentence);

    speech.volume = 1;
    speech.pitch = 1;
    speech.rate = 1;

    window.speechSynthesis.speak(speech);
}

window.addEventListener('load', ()=>{
    speak("Welcome Home User");
})

const SpeechRecognition = window.SpeechRecognition || window.webkitSpeechRecognition;
const recognition = new SpeechRecognition();

recognition.onresult = (event) => {
    const current = event.resultIndex;
    const transcript = event.results[current][0].transcript;
    content.textContent = transcript;
    setTimeout(function () {
        content.textContent = "Click here to speak"; 
    }, 5000);
    speakThis(transcript.toLowerCase());
}

micBtn.addEventListener('click', ()=>{
    recognition.start();
})

function speakThis(message) {

    if(message.includes('lighting mode') || message.includes('light mode')) {
        if(message.includes('manual') || message.includes('switch only')) {
            setBedRoomLightingMode(0);
            const sentence = 'Setting bedroom\'s lighting mode to manual';
            speak(sentence);
        }
        else if(message.includes('automatic')) {
            setBedRoomLightingMode(1);
            const sentence = 'Setting bedroom\'s lighting mode to automatic';
            speak(sentence);
        }
        else if(message.includes('shedule') || message.includes('scheduled')) {
            setBedRoomLightingMode(2);
            const sentence = 'Setting bedroom\'s lighting mode to scheduled';
            speak(sentence);
        }
    }

    else if((message.includes('bedroom') || message.includes('bed')) && message.includes('light') && message.includes('on')) {
        setBedRoomLight(true);
        const sentence = "Turning on bedroom light";
        speak(sentence);
    }

    else if((message.includes('bedroom') || message.includes('bed')) && message.includes('light') && message.includes('off')) {
        setBedRoomLight(false);
        const sentence = "Turning off bedroom light";
        speak(sentence);
    }

    else if((message.includes('livingroom') || message.includes('living') || message.includes('livin')) && message.includes('light') && message.includes('on')) {
        setLivingRoomLight(true);
        const sentence = "Turning on living room light";
        speak(sentence);
    }

    else if((message.includes('livingroom') || message.includes('bed')  || message.includes('livin')) && message.includes('light') && message.includes('off')) {
        setLivingRoomLight(false);
        const sentence = "Turning off living room light";
        speak(sentence);
    }

    else if(message.includes('environment') || message.includes('temperature') || message.includes('humidity')) {
        const sentence = `It is ${temperature} degree celcius and ${humidity} percent humidity`;
        speak(sentence);
    }

}

// Websockets ****************************************************************************************

var socket;

socket = new WebSocket('ws://192.168.0.1:81/');

socket.onopen = function() {
    console.log('WebSocket connection opened');
};

displayBedRoomLight(false);
displayBedRoomLightingMode();
displayLivingRoomLight(false);

socket.onmessage = function(event) {
    console.log(event.data);

    const data = JSON.parse(event.data);
    for (const key in data) {
        if(key == 'bedRoomLight') {
            displayBedRoomLight(data.bedRoomLight);
        }
        else if(key == 'bedRoomLightingMode') {
            bedRoomLightingMode = data.bedRoomLightingMode;
            displayBedRoomLightingMode();
        }
        else if(key == 'livingRoomLight') {
            displayLivingRoomLight(data.livingRoomLight);
        }
        else if(key == 'environment') {
            temperature = data.environment[0];
            humidity = data.environment[1];
        }
        else if(key == 'firebase') {
            window.location.href = "https://smart-home-project.web.app/";
        }
    }
};

socket.onclose = function() {
    console.log('WebSocket connection closed');
};
