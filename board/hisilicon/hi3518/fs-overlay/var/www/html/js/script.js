// common variables
var iBytesUploaded = 0;
var iBytesTotal = 0;
var iPreviousBytesLoaded = 0;
var iMaxFilesize = 16 * 1048576; // 16MB
var oTimer = 0;
var sResultFileSize = '';

window.onload = function() {
    document.getElementById('file').removeAttribute('disabled');
    document.getElementById('upload_btn').setAttribute('disabled', 'disabled');
    if(document.getElementById('file').files[0])
        fileSelected();
    //document.getElementById('upload_form').reset();
};

// convert seconds in normal time format
function secondsToTime(secs) {
    var hr = Math.floor(secs / 3600);
    var min = Math.floor((secs - (hr * 3600))/60);
    var sec = Math.floor(secs - (hr * 3600) -  (min * 60));

    if (hr < 10) {hr = "0" + hr; }
    if (min < 10) {min = "0" + min;}
    if (sec < 10) {sec = "0" + sec;}
    if (hr) {hr = "00";}
    return hr + ':' + min + ':' + sec;
};

function bytesToSize(bytes) {
    var sizes = ['Bytes', 'KB', 'MB'];
    if (bytes == 0) return 'n/a';
    var i = parseInt(Math.floor(Math.log(bytes) / Math.log(1024)));
    return (bytes / Math.pow(1024, i)).toFixed(1) + ' ' + sizes[i];
};

function logOutput(string) {
    var p = document.createElement("p");
    p.setAttribute('class', 'output');
    p.innerHTML = string;
    document.getElementById("log_output").appendChild(p);
    document.getElementById("log_output").style.display = 'block';
    return p;
}

function fileSelected() {
    document.getElementById('fileinfo').style.display = 'none';
    document.getElementById('progress_info').style.display = 'none';
    // hide different warnings
    document.getElementById('log_output').style.display = 'none';
    document.getElementById('error').style.display = 'none';
    document.getElementById('upload_btn').disabled = 'disabled';

    // get selected file element
    var oFile = document.getElementById('file').files[0];

    // little test for filesize
    if (oFile.size > iMaxFilesize) {
        var oError = document.getElementById('error')
        oError.innerHTML = 'File is too large. Please select a valid firmware';
        oError.style.display = 'block';
        return;
    }

    document.getElementById('upload_btn').removeAttribute('disabled');
    document.getElementById('error').innerHTML = '';

    document.getElementById('fileinfo').style.display = 'block';
    document.getElementById('filename').innerHTML = 'Name: ' + oFile.name;
    document.getElementById('filesize').innerHTML = 'Size: ' + bytesToSize(oFile.size);
    document.getElementById('filetype').innerHTML = 'Type: ' + oFile.type;
}

function startUploading() {
    document.getElementById('log_output').style.display = 'none';
    document.getElementById('log_output').innerHTML = '';

    doPostMethod({
        'prompt':'Enter download mode',
        'uri':'/cgi-bin/upgrade.cgi',
        'formdata': 'action=switchmode'
    }, function(e) {
        doUpload({
            'prompt':'Uploading firmware',
            'uri':'/cgi-bin/upload.cgi',
        }, function(e) {
            var resp = JSON.parse(e.target.responseText);
            var file = resp.file;
            doPostMethod({
                'prompt':'Programming firmware',
                'uri':'/cgi-bin/upgrade.cgi',
                'formdata': 'action=upgrade&file=' + encodeURIComponent(file)
            }, function(e) {
                var p = logOutput('Upgrade complete, now rebooting...');
                p.style.color = '#0a0';
                p.style['font-size'] = '14pt';
                document.getElementById('file').removeAttribute('disabled');
                document.getElementById('upload_btn').removeAttribute('disabled');
            });
        });
    });
}

function doUpload(request, onSuccess) {
    var p = logOutput(request.prompt);
    // cleanup all temp states
    iPreviousBytesLoaded = 0;
    document.getElementById('error').style.display = 'none';
    document.getElementById('progress_info').style.display = 'block';
    document.getElementById('progress_percent').innerHTML = '0%';
    var oProgress = document.getElementById('progress');
    oProgress.style.display = 'block';
    oProgress.value = '0';

    var vFD = new FormData(document.getElementById('upload_form'));

    var oXHR = new XMLHttpRequest();
    oXHR.upload.addEventListener('progress', function(e) {
        if (e.lengthComputable) {
            iBytesUploaded = e.loaded;
            iBytesTotal = e.total;
            var iPercentComplete = Math.round(e.loaded * 100 / e.total);
            var iBytesTransfered = bytesToSize(iBytesUploaded);

            document.getElementById('progress_percent').innerHTML =
                iPercentComplete.toString() + '%';
            document.getElementById('progress').value = iPercentComplete;
            document.getElementById('b_transfered').innerHTML = iBytesTransfered;
        } else {
            document.getElementById('progress').innerHTML = 'unable to compute';
        }
    }, false);
    oXHR.addEventListener('load', function(e) {
        document.getElementById('progress_percent').innerHTML = '100%';
        document.getElementById('progress').value = '100';
        document.getElementById('remaining').innerHTML = '| 00:00:00';

        clearInterval(oTimer);

        if (e.target.status == '200') {
            var resp = JSON.parse(e.target.responseText);
	    if (resp.status == 'OK') {
                p.innerHTML += ' [OK]';
                onSuccess(e);
                return;
            }
        }

        p.innerHTML += ' [FAIL]';
        document.getElementById('file').removeAttribute('disabled');
        document.getElementById('upload_btn').removeAttribute('disabled');
    }, false);
    oXHR.addEventListener('error', function(e) {
        p.innerHTML += ' [ERROR]';
        document.getElementById('error').style.display = 'block';
        document.getElementById('error').innerHTML =
            'Error occurred while uploading ';
        document.getElementById('file').removeAttribute('disabled');
        document.getElementById('upload_btn').removeAttribute('disabled');
        clearInterval(oTimer);
    }, false);
    oXHR.addEventListener('abort', function(e) {
        p.innerHTML += ' [ABORT]';
        document.getElementById('error').style.display = 'block';
        document.getElementById('error').innerHTML =
            'The upload has been canceled';
        document.getElementById('file').removeAttribute('disabled');
        document.getElementById('upload_btn').removeAttribute('disabled');
        clearInterval(oTimer);
    }, false);
    oXHR.open('POST', document.getElementById('upload_form').action);
    oXHR.send(vFD);

    // set inner timer
    oTimer = setInterval(function() {
        var iCB = iBytesUploaded;
        var iDiff = iCB - iPreviousBytesLoaded;
        // if nothing new loaded - exit
        if (iDiff == 0)
            return;
        iPreviousBytesLoaded = iCB;
        iDiff = iDiff * 2;
        var iBytesRem = iBytesTotal - iPreviousBytesLoaded;
        var secondsRemaining = iBytesRem / iDiff;
        // update speed info
        var iSpeed = iDiff.toString() + 'B/s';
        if (iDiff > 1024 * 1024) {
            iSpeed = (Math.round(iDiff * 100/(1024*1024))/100).toString() + 'MB/s';
        } else if (iDiff > 1024) {
            iSpeed =  (Math.round(iDiff * 100/1024)/100).toString() + 'KB/s';
        }
        document.getElementById('speed').innerHTML = iSpeed;
        document.getElementById('remaining').innerHTML = '| '
            + secondsToTime(secondsRemaining);
    }, 300);
    document.getElementById('file').disabled = 'disabled';
    document.getElementById('upload_btn').setAttribute('disabled', 'disabled');
}

function doPostMethod(request, onSuccess) {
    var p = logOutput(request.prompt);
    var oXHR = new XMLHttpRequest();
    oXHR.addEventListener('load', function(e) {
        clearInterval(oTimer);
        if (e.target.status  == '200') {
            var resp = JSON.parse(e.target.responseText);
            if (resp.status == 'OK') {
                p.innerHTML += ' [OK]';
                onSuccess(e);
                return;
	    }
            else {
                logOutput(resp.errmsg);
            }
        }
        p.innerHTML += ' [FAIL]';
        document.getElementById('file').removeAttribute('disabled');
        document.getElementById('upload_btn').removeAttribute('disabled');
    });
    oXHR.addEventListener('error', function(e) {
        p.innerHTML += 'ERROR';
        clearInterval(oTimer);
    });
    oXHR.addEventListener('abort', function(e) {
        p.innerHTML += 'ABORT';
        clearInterval(oTimer);
    });
    oXHR.open("POST", request.uri);
    oXHR.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    oXHR.send(request.formdata);
    oTimer = setInterval(function() {
        p.innerHTML += ".";
    }, 1000);
}
