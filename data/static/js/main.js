var host = (window.location.protocol !== 'file:') ? window.location.host : "wemos"; // change "wemos" to your ip/domain for debugging

var controlSelector = '#controls';
var settingsSelector = '#settings';

var IoT_Control_LED = "CONTROL_LED";
var setControlValuesRequest;

function onLoad() {
    getTitle();
    getValues();

    initWebSocket();
}

function getTitle() {
    $.get('http://' + host + '/title', function (response) {
        window.document.title = title;
        $('#title').html(title);
    });
}

function getValues() {
    $.get('http://' + host + '/get', function (response) {
        initGui(response);
    });
}

function initGui(data) {
    $.each(data, function (controlId, controlData) {
        switch (controlData.type) {
            case "button":
                initButton(controlId, controlData);
                break;
            case "checkbox":
                initCheckbox(controlId, controlData);
                break;
            case "slider":
                initSlider(controlId, controlData);
                break;
        }
    });
    $('#loading').remove();
}

function initWebSocket() {
    new WebSocket('ws://' + host + ':8080').addEventListener('message', function (event) {
        updateGui(JSON.parse(event.data));
    });
}

function updateGui(data) {
    $.each(data, function (controlId, controlData) {
        switch (controlData.type) {
            case "button":
                updateButton(controlId, controlData);
                break;
            case "checkbox":
                updateCheckbox(controlId, controlData);
                break;
            case "slider":
                updateSlider(controlId, controlData);
                break;
        }
    });
}

function setValues(data) {
    if (setControlValuesRequest !== undefined) {
        return;
    }

    setControlValuesRequest = $.post('http://' + host + '/set', data, function (response) {
        updateGui(response);
        setControlValuesRequest = undefined;
    });
}