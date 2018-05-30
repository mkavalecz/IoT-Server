var host = (window.location.protocol !== 'file:') ? window.location.host : "wemos"; // change "wemos" to your ip/domain for debugging
var setControlValuesRequest;

var IoT_Control_LED = "CONTROL_LED";

function onLoad() {
    getValues();

    new WebSocket('ws://' + host + ':8080').addEventListener('message', function (event) {
        updateGui(JSON.parse(event.data));
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
            case "led":
                initLED(controlId, controlData);
        }
    });
    $('#loading').remove();
}

function initButton(controlId, controlData) {
    var valueInput = $('#' + controlId + 'Value');
    if (!valueInput.length) {
        $('#containerForm').append(
            '<div class="form-row">' +
            '<div class="col form-group text-center">' +
            '<label for="' + controlId + 'Value" class="font-weight-bold">' + controlData.name + '</label>' +
            '<div class="w-100"></div>' +
            '<button id="' + controlId + 'Value"  type="button" class="btn"></button>' +
            '</div>' +
            '</div>'
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
        $('#containerForm').append(
            '<div class="form-row">' +
            '<div class="col form-group text-center">' +
            '<label for="' + controlId + 'Value" class="font-weight-bold">' + controlData.name + '</label>' +
            '<input id="' + controlId + 'Value" type="range" class="form-control bg-secondary" min="0" , max="' + controlData.maxBrightness + '" />' +
            '<p id="' + controlId + 'Text" class="form-text">N/A</p>' +
            '</div>' +
            '</div>'
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
            case "led":
                $('#' + controlId + 'Value').val(controlData.value);
                $('#' + controlId + 'Text').text(controlData.value);
        }
    });
}