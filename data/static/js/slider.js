var sliderTemplate = '<div class="form-group">' +
    '<div class="row text-center">' +
    '<div class="col form-group">' +
    '<label for="#id#Slider" class="font-weight-bold">#name#</label>' +
    '<input id="#id#Slider" type="range" class="form-control bg-secondary" min="#minValue#" , max="#maxValue#" />' +
    '</div>' +
    '</div>' +
    '<div class="row justify-content-center text-center">' +
    '<div class="col-4 form-group">' +
    '<input id="#id#Text" type="text" class="form-control text-center" />' +
    '</div>' +
    '</div>' +
    '<div class="row justify-content-center text-center">' +
    '<div class="col form-group">' +
    '<button id="#id#SaveButton" type="button" class="btn btn-primary">Save</button>' +
    '</div>' +
    '</div>' +
    '<div class="row justify-content-center text-center">' +
    '<div class="col form-group">' +
    '<span id="#id#SaveState" class="save-state"></span>' +
    '</div>' +
    '</div>' +
    '</div>';

function initSlider(controlId, controlData) {
    if (controlId === IoT_Control_LED) {
        return;
    }

    var valueSlider = $('#' + controlId + 'Slider');
    var valueInput = $('#' + controlId + 'Text');
    var saveButton = $('#' + controlId + 'SaveButton');
    if (!valueSlider.length && !valueInput.length && !saveButton.length) {
        var selector = controlSelector;
        if (controlData.showOnSettings) {
            selector = settingsSelector;
        }
        $(selector).append(sliderTemplate
            .replace(new RegExp('#id#', 'g'), controlId)
            .replace(new RegExp('#name#', 'g'), controlData.name)
            .replace(new RegExp('#minValue#', 'g'), controlData.minValue)
            .replace(new RegExp('#maxValue#', 'g'), controlData.maxValue)
        );
        valueSlider = $('#' + controlId + 'Slider');
        valueInput = $('#' + controlId + 'Text');
        saveButton = $('#' + controlId + 'SaveButton');
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
    saveButton.on('mouseup', function () {
        var data = {};
        data[controlId] = true;
        saveValues(data);
    });
}

function updateSlider(controlId, controlData) {
    $('#' + controlId + 'Slider').val(controlData.value);
    $('#' + controlId + 'Text').val(controlData.value);
}

function showSliderSaveResults(controlId, success) {
    if (success) {
        $('#' + controlId + 'SaveState').css('color', 'green');
        $('#' + controlId + 'SaveState').text('Save successful.');
        setTimeout(function () {
            $('#' + controlId + 'SaveState').text('');
        }, 3000);
    } else {
        $('#' + controlId + 'SaveState').css('color', 'red');
        $('#' + controlId + 'SaveState').text('Save failed.');
    }
}