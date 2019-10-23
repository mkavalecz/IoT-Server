var checkboxTemplate = '<div class="form-group">' +
    '<div class="row text-center">' +
    '<div class="col form-group">' +
    '<label for="#id#Value" class="font-weight-bold">#name#</label>' +
    '</div>' +
    '</div>' +
    '<div class="row text-center">' +
    '<div class="col form-group">' +
    '<input id="#id#Value" type="checkbox" />' +
    '</div>' +
    '</div>' +
    '<div class="row justify-content-center">' +
    '<div class="form-group">' +
    '<button id="#id#SaveButton" type="button" class="btn btn-primary">Save</button>' +
    '</div>' +
    '</div>' +
    '<div class="row justify-content-center">' +
    '<div class="form-group">' +
    '<span id="#id#SaveState" class="save-state"></span>' +
    '</div>' +
    '</div>' +
    '</div>';

function initCheckbox(controlId, controlData) {
    var valueInput = $('#' + controlId + 'Value');
    var saveButton = $('#' + controlId + 'SaveButton');
    if (!valueInput.length && !saveButton.length) {
        var selector = controlSelector;
        if (controlData.showOnSettings) {
            selector = settingsSelector;
        }
        $(selector).append(checkboxTemplate
            .replace(new RegExp('#id#', 'g'), controlId)
            .replace(new RegExp('#name#', 'g'), controlData.name)
        );
        valueInput = $('#' + controlId + 'Value');
        var saveButton = $('#' + controlId + 'SaveButton');
    }
    valueInput.bootstrapToggle({
        onstyle: 'success',
        offstyle: 'danger',
        size: 'lg'
    });
    valueInput.prop('checked', controlData.value != 0).change();
    valueInput.on('change', function () {
        var data = {};
        data[controlId] = valueInput.prop('checked') ? 1 : 0;
        setValues(data);
    });
    saveButton.on('mouseup', function () {
        var data = {};
        data[controlId] = true;
        saveValues(data);
    });
}

function updateCheckbox(controlId, controlData) {
    $('#' + controlId + 'Value').prop('checked', controlData.value === 1);
}

function showCheckboxSaveResults(controlId, success) {
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