var checkboxTemplate = '<div class="form-group">' +
    '<div class="row text-center">' +
    '<div class="col form-group">' +
    '<label for="#id#Value" class="font-weight-bold">#name#</label>' +
    '<div class="w-100"></div>' +
    '<input id="#id#Value" type="checkbox" />' +
    '</div>' +
    '</div>' +
    '</div>';

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
}

function updateCheckbox(controlId, controlData) {
    $('#' + controlId + 'Value').prop('checked', controlData.value === 1);
}