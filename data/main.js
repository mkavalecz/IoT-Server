var host = (window.location.protocol !== 'file:') ? window.location.host : "wemos"; // change "wemos" to your ip/domain for debugging

var buttonTemplate = '<div class="form-row">' +
    '<div class="col form-group text-center">' +
    '<label for="#id#Value" class="font-weight-bold">#name#</label>' +
    '<div class="w-100"></div>' +
    '<button id="#id#Value" type="button" class="btn"></button>' +
    '</div>' +
    '</div>';

var ledTemplate = '<div class="form-row">' +
    '<div class="col form-group text-center">' +
    '<label for="#id#Value" class="font-weight-bold">#name#</label>' +
    '<input id="#id#Value" type="range" class="form-control bg-secondary" min="0" , max="#maxBrightness#" />' +
    '<p id="#id#Text" class="form-text">N/A</p>' +
    '</div>' +
    '</div>';

var sliderTemplate = '<div class="form-row">' +
    '<div class="col form-group text-center">' +
    '<label for="#id#Value" class="font-weight-bold">#name#</label>' +
    '<input id="#id#Value" type="range" class="form-control bg-secondary" min="#minValue#" , max="#maxValue#" />' +
    '<p id="#id#Text" class="form-text">N/A</p>' +
    '</div>' +
    '</div>';

var containerSelector = '#containerForm';

var IoT_Control_LED = "CONTROL_LED";
var setControlValuesRequest;

function onLoad() {
    getTitle();
    getValues();

    new WebSocket('ws://' + host + ':8080').addEventListener('message', function (event) {
        updateGui(JSON.parse(event.data));
    });
}

function getTitle() {
    $.get('http://' + host + '/title', function (response) {
        setTitle(response.title);
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
            case "led":
                initLED(controlId, controlData);
                break;
            case "slider":
                initSlider(controlId, controlData);
                break;
        }
    });
    $('#loading').remove();
}

function setTitle(title) {
    window.document.title = title;
    $('#title').html(title);
}

function initButton(controlId, controlData) {
    var valueInput = $('#' + controlId + 'Value');
    if (!valueInput.length) {
        $(containerSelector).append(buttonTemplate
            .replace(new RegExp("#id#", 'g'), controlId)
            .replace(new RegExp("#name#", 'g'), controlData.name)
        );
        valueInput = $('#' + controlId + 'Value');
    }
    valueInput.val(controlData.value);
    valueInput.on('mousedown mouseup', function () {
        var data = {};
        data[controlId] = (event.type === 'mousedown') ? 1 : 0;
        setValues(data);
    });
}

function initLED(controlId, controlData) {
    if (controlId === IoT_Control_LED) {
        return;
    }

    var valueInput = $('#' + controlId + 'Value');
    var textOutput = $('#' + controlId + 'Text');
    if (!valueInput.length && !textOutput.length) {
        $(containerSelector).append(ledTemplate
            .replace(new RegExp("#id#", 'g'), controlId)
            .replace(new RegExp("#name#", 'g'), controlData.name)
            .replace(new RegExp("#maxBrightness#", 'g'), controlData.maxBrightness)
        );
        valueInput = $('#' + controlId + 'Value');
        textOutput = $('#' + controlId + 'Text');
    }
    valueInput.val(controlData.value);
    textOutput.text(controlData.value);
    valueInput.on('input change', function () {
        var data = {};
        data[controlId] = this.value;
        setValues(data);
    });
}

function initSlider(controlId, controlData) {
    var valueInput = $('#' + controlId + 'Value');
    var textOutput = $('#' + controlId + 'Text');
    if (!valueInput.length && !textOutput.length) {
        $(containerSelector).append(sliderTemplate
            .replace(new RegExp("#id#", 'g'), controlId)
            .replace(new RegExp("#name#", 'g'), controlData.name)
            .replace(new RegExp("#minValue#", 'g'), controlData.minValue)
            .replace(new RegExp("#maxValue#", 'g'), controlData.maxValue)
        );
        valueInput = $('#' + controlId + 'Value');
        textOutput = $('#' + controlId + 'Text');
    }
    valueInput.val(controlData.value);
    textOutput.text(controlData.value);
    valueInput.on('input change', function () {
        var data = {};
        data[controlId] = this.value;
        setValues(data);
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

function updateGui(data) {
    $.each(data, function (controlId, controlData) {
        switch (controlData.type) {
            case "button":
                if (controlData.value) {
                    $('#' + controlId + 'Value').addClass("active");
                } else {
                    $('#' + controlId + 'Value').removeClass("active");
                }
                break;
            case "led":
            case "slider":
                $('#' + controlId + 'Value').val(controlData.value);
                $('#' + controlId + 'Text').text(controlData.value);
                break;
        }
    });
}