var buttonTemplate = '<div class="form-group">' +
    '<div class="row justify-content-center text-center">' +
    '<div class="col form-group">' +
    '<label for="#id#Value" class="font-weight-bold">#name#</label>' +
    '</div>' +
    '</div>' +
    '<div class="row justify-content-center text-center">' +
    '<div class="col form-group">' +
    '<button id="#id#Value" type="button" class="btn big-btn"></button>' +
    '</div>' +
    '</div>' +
    '</div>';

function initButton(controlId, controlData) {
    var valueInput = $('#' + controlId + 'Value');
    if (!valueInput.length) {
        var selector = controlSelector;
        if (controlData.showOnSettings) {
            selector = settingsSelector;
        }
        $(selector).append(buttonTemplate
            .replace(new RegExp('#id#', 'g'), controlId)
            .replace(new RegExp('#name#', 'g'), controlData.name)
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

function updateButton(controlId, controlData) {
    if (controlData.value) {
        $('#' + controlId + 'Value').addClass('active');
    } else {
        $('#' + controlId + 'Value').removeClass('active');
    }
}
