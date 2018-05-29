var host = 'wemos'; // change this

var getValuesRequest;
var setValuesRequest;
var updateSocket;

function onLoad() {
    getValues();

    updateSocket = new WebSocket('ws://' + host + ':8080');
    updateSocket.addEventListener('message', function (event) {
        updateGui(JSON.parse(event.data));
    });

    $('#led1Value').on('input change', function () {
        setValues({
            led1: this.value
        });
    });
    $('#led2Value').on('input change', function () {
        setValues({
            led2: this.value
        });
    });
    $('#button1Value').on('mousedown mouseup', function (event) {
        setValues({
            button1: (event.type === 'mousedown') ? 1 : 0
        });
    });
}

function getValues() {
    if (getValuesRequest !== undefined) {
        return;
    }

    getValuesRequest = $.get('http://' + host + '/get', function (response) {
        updateGui(response);
        getValuesRequest = undefined;
    });
}

function setValues(data) {
    if (setValuesRequest !== undefined) {
        return;
    }

    setValuesRequest = $.post('http://' + host + '/set', data, function (response) {
        updateGui(response);
        setValuesRequest = undefined;
    });
}

function updateGui(data) {
    if (data.led1 !== undefined) {
        $('#led1Value').val(data.led1);
        $('#led1Text').text(data.led1);
    }
    if (data.led2 !== undefined) {
        $('#led2Value').val(data.led2);
        $('#led2Text').text(data.led2);
    }
    if (data.button1 !== undefined) {
        if (data.button1) {
            $('#button1').addClass("active");
        } else {
            $('#button1').removeClass("active");
        }
    }
}