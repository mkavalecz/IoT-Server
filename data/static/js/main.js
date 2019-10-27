var host = (window.location.protocol !== 'file:') ? window.location.host : 'wemos'; // change "wemos" to your ip/domain for debugging
var protocol = window.location.protocol + '//';

var controlSelector = '#controls';
var settingsSelector = '#settings';

var IoT_Control_LED = 'CONTROL_LED';
var setControlValuesRequest;
var saveControlValuesRequest;

var controls;

var saveAllButtonTemplate = '<div class="col form-group text-center">' +
    '<button id="saveSettings" type="button" class="btn btn-primary" onclick="saveAll(\'#id#\')">Save All</button>' +
    '</div>';

function onLoad() {
    getTitle();
    getValues();

    initWebSocket();
}

function getTitle() {
    $.get(protocol + host + '/title', function (response) {
        window.document.title = response.title;
        $('#title').html(response.title);
    });
}

function getValues() {
    $.get(protocol + host + '/get', function (response) {
        controls = response;
        initGui();
    });
}

function initGui() {
    $.each(controls, function (controlId, controlData) {
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
    $(controlSelector).append(saveAllButtonTemplate.replace(new RegExp('#id#', 'g'), controlSelector));
    $(settingsSelector).append(saveAllButtonTemplate.replace(new RegExp('#id#', 'g'), settingsSelector));
}

function initWebSocket() {
    new WebSocket(protocol === 'https://' ? ('wss://' + host + "/ws") : ('ws://' + host + ':8080')).addEventListener('message', function (event) {
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

    setControlValuesRequest = $.post(protocol + host + '/set', data, function (response) {
        updateGui(response);
    }).always(function () {
        setControlValuesRequest = undefined;
    });
}

function saveAll(container) {
    var data = {};
    for (var controlId in controls) {
        if (!controls.hasOwnProperty(controlId) || !controls[controlId]) {
            continue;
        }

        var controlData = controls[controlId];

        if (controlId === IoT_Control_LED ||
            (container === '#controls' && controlData.showOnSettings) ||
            (container === '#settings' && !controlData.showOnSettings)
        ) {
            continue;
        }
        switch (controlData.type) {
            case "checkbox":
            case "slider":
                data[controlId] = true;
                break;
        }
    }
    saveValues(data);
}

function saveValues(data) {
    if (saveControlValuesRequest !== undefined) {
        return;
    }

    saveControlValuesRequest = $.post(protocol + host + '/save', data, function (response) {
        showSaveResults(response);
    }).always(function () {
        saveControlValuesRequest = undefined;
    });
}

function showSaveResults(data) {
    $.each(data, function (controlId, success) {
        switch (controls[controlId].type) {
            case "checkbox":
                showCheckboxSaveResults(controlId, success);
                break;
            case "slider":
                showSliderSaveResults(controlId, success);
                break;
        }
    });
}