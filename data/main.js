var host = (window.location.protocol !== 'file:') ? window.location.host : "wemos"; // change "wemos" to your ip/domain for debugging

var buttonTemplate = '<div class="form-group">' +
    '<div class="row text-center">' +
    '<div class="col form-group">' +
    '<label for="#id#Value" class="font-weight-bold">#name#</label>' +
    '<div class="w-100"></div>' +
    '<button id="#id#Value" type="button" class="btn big-btn"></button>' +
    '</div>' +
    '</div>' +
    '</div>';

var checkboxTemplate = '<div class="form-group">' +
    '<div class="row text-center">' +
    '<div class="col form-group">' +
    '<label for="#id#Value" class="font-weight-bold">#name#</label>' +
    '<div class="w-100"></div>' +
    '<input id="#id#Value" type="checkbox" />' +
    '</div>' +
    '</div>' +
    '</div>';

var sliderTemplate = '<div class="form-group">' +
    '<div class="row text-center">' +
    '<div class="col form-group">' +
    '<label for="#id#Slider" class="font-weight-bold">#name#</label>' +
    '<input id="#id#Slider" type="range" class="form-control bg-secondary" min="#minValue#" , max="#maxValue#" />' +
    '</div>' +
    '</div>' +
    '<div class="row justify-content-center">' +
    '<div class="col-4 form-group">' +
    '<input id="#id#Text" type="text" class="form-control text-center" />' +
    '</div>' +
    '</div>' +
    '</div>';

var controlSelector = '#controls';
var settingsSelector = '#settings';

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

function setTitle(title) {
    window.document.title = title;
    $('#title').html(title);
}

function initButton(controlId, controlData) {
    var valueInput = $('#' + controlId + 'Value');
    if (!valueInput.length) {
        var selector = controlSelector;
        if (controlData.showOnSettings) {
            selector = settingsSelector;
        }
        $(selector).append(buttonTemplate
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

function initCheckbox(controlId, controlData) {
    var valueInput = $('#' + controlId + 'Value');
    if (!valueInput.length) {
        var selector = controlSelector;
        if (controlData.showOnSettings) {
            selector = settingsSelector;
        }
        $(selector).append(checkboxTemplate
            .replace(new RegExp("#id#", 'g'), controlId)
            .replace(new RegExp("#name#", 'g'), controlData.name)
        );
        valueInput = $('#' + controlId + 'Value');
    }
    valueInput.bootstrapToggle({
        onstyle: 'primary',
        offstyle: 'secondary',
        size: 'lg'
    });
    valueInput.prop('checked', controlData.value != 0).change();
    valueInput.on('change', function () {
        var data = {};
        data[controlId] = valueInput.prop('checked') ? 1 : 0;
        setValues(data);
    });
}

function initSlider(controlId, controlData) {
    if (controlId === IoT_Control_LED) {
        return;
    }

    var valueSlider = $('#' + controlId + 'Slider');
    var valueInput = $('#' + controlId + 'Text');
    if (!valueSlider.length && !valueInput.length) {
        var selector = controlSelector;
        if (controlData.showOnSettings) {
            selector = settingsSelector;
        }
        $(selector).append(sliderTemplate
            .replace(new RegExp("#id#", 'g'), controlId)
            .replace(new RegExp("#name#", 'g'), controlData.name)
            .replace(new RegExp("#minValue#", 'g'), controlData.minValue)
            .replace(new RegExp("#maxValue#", 'g'), controlData.maxValue)
        );
        valueSlider = $('#' + controlId + 'Slider');
        valueInput = $('#' + controlId + 'Text');
    }
    valueSlider.val(controlData.value);
    valueInput.val(controlData.value);
    valueSlider.on('input change', function () {
        var data = {};
        data[controlId] = this.value;
        setValues(data);
    });
    valueInput.on('change', function () {
        var num = parseInt(valueInput.val());
        if (num === NaN || num < controlData.minValue) {
            num = controlData.minValue;
        } else if (num > controlData.maxValue) {
            num = controlData.maxValue;
        }
        var data = {};
        data[controlId] = num;
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
            case "slider":
                $('#' + controlId + 'Slider').val(controlData.value);
                $('#' + controlId + 'Text').val(controlData.value);
                break;
        }
    });
}