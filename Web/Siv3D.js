mergeInto(LibraryManager.library, {
    //
    // GamePads
    //
    siv3dGetJoystickInfo: function(joystickId) {
        return GLFW.joys[joystickId].id;
    },
    siv3dGetJoystickInfo__sig: "iiiii",

    glfwGetJoystickHats: function () {
        // Not supported.
        return 0;
    },
    glfwGetJoystickHats__sig: "iii",

    glfwGetKeysSiv3D: function (windowid) {
        const window = GLFW.WindowFromId(windowid);
        if (!window) return 0;
        if (!window.keysBuffer) {
            window.keysBuffer = Module._malloc(349 /* GLFW_KEY_LAST + 1 */)
        }
        Module.HEAPU8.set(window.keys, window.keysBuffer);
        return window.keysBuffer;
    },
    glfwGetKeysSiv3D__sig: "ii",

    //
    // Monitors
    //
    glfwGetMonitorInfo_Siv3D: function(handle, displayID, xpos, ypos, w, h) {
        setValue(displayID, 1, 'i32');
        setValue(xpos, 0, 'i32');
        setValue(ypos, 0, 'i32');
        setValue(w, 0, 'i32');
        setValue(h, 0, 'i32');
    },
    glfwGetMonitorInfo_Siv3D__sig: "viiiiiiiiiii",

    glfwGetMonitorWorkarea: function(handle, wx, wy, ww, wh) {
        setValue(wx, 0, 'i32');
        setValue(wy, 0, 'i32');
        setValue(ww, 1280, 'i32');
        setValue(wh, 720, 'i32');
    },
    glfwGetMonitorWorkarea__sig: "viiiii",

    glfwGetMonitorContentScale: function(handle, xscale, yscale) {
        setValue(xscale, 1, 'float');
        setValue(yscale, 1, 'float'); 
    },
    glfwGetMonitorContentScale__sig: "viii",

    siv3dSetCursorStyle: function(style) {
        const styleText = UTF8ToString(style);
        Module["canvas"].style.cursor = styleText;
    },
    siv3dSetCursorStyle__sig: "vi",

    //
    // MessageBox
    //
    siv3dShowMessageBox: function(messagePtr, type) {
        const message = UTF8ToString(messagePtr);

        if (type === 0) {
            /* MessageBoxButtons.OK */
            window.alert(message);
            return 0; /* MessageBoxResult.OK */
        } else if (type === 1) {
            /* MessageBoxButtons.OKCancel */
            return window.confirm(message) ? 0 /* MessageBoxResult.OK */ : 1 /* MessageBoxResult.Cancel */;
        }

        return 4; /* MessageBoxSelection.None */
    },
    siv3dShowMessageBox__sig: "iii",

    //
    // DragDrop Support
    //
    siv3dRegisterDragEnter: function(ptr) {
        Module["canvas"].ondragenter = function (e) {
            e.preventDefault();

            const types = e.dataTransfer.types;

            if (types.length > 0) {
                {{{ makeDynCall('vi', 'ptr') }}}(types[0] === 'Files' ? 1 : 0);
            }        
        };
    },
    siv3dRegisterDragEnter__sig: "vi",

    siv3dRegisterDragUpdate: function(ptr) {
        Module["canvas"].ondragover = function (e) {
            e.preventDefault();
            {{{ makeDynCall('v', 'ptr') }}}();
        };
    },
    siv3dRegisterDragUpdate__sig: "vi",

    siv3dRegisterDragExit: function(ptr) {
        Module["canvas"].ondragexit = function (e) {
            e.preventDefault();
            {{{ makeDynCall('v', 'ptr') }}}();
        };
    },
    siv3dRegisterDragExit__sig: "vi",

    $siv3dDragDropFileReader: null,
    siv3dRegisterDragDrop: function(ptr) {
        Module["canvas"].ondrop = function (e) {
            e.preventDefault();

            const items = e.dataTransfer.items;

            if (items.length == 0) {
                return;
            }

            if (items[0].kind === 'text') {
                items[0].getAsString(function(str) {
                    const strPtr = allocate(intArrayFromString(str), ALLOC_NORMAL);
                    {{{ makeDynCall('vi', 'ptr') }}}(strPtr);
                    Module["_free"](strPtr);
                })            
            } else if (items[0].kind === 'file') {
                const file = items[0].getAsFile();

                if (!siv3dDragDropFileReader) {
                    siv3dDragDropFileReader = new FileReader();
                }

                const filePath = `/tmp/${file.name}`;

                siv3dDragDropFileReader.addEventListener("load", function onLoaded() {
                    FS.writeFile(filePath, new Uint8Array(siv3dDragDropFileReader.result));

                    const namePtr = allocate(intArrayFromString(filePath), ALLOC_NORMAL);
                    {{{ makeDynCall('vi', 'ptr') }}}(namePtr);

                    siv3dDragDropFileReader.removeEventListener("load", onLoaded);
                });

                siv3dDragDropFileReader.readAsArrayBuffer(file);              
            }
        };
    },
    siv3dRegisterDragDrop__sig: "vi",
    siv3dRegisterDragDrop__deps: [ "$siv3dDragDropFileReader", "$FS" ],

    //
    // WebCamera/Movie Support
    //
    $videoElements: [],

    siv3dOpenVideo: function(fileName) {

    },
    siv3dOpenVideo__sig: "vi",

    siv3dOpenCamera: function(width, height, callback, callbackArg) {
        const constraint = {
            video: { width, height },
            audio: false
        };

        navigator.mediaDevices.getUserMedia(constraint).then(
            stream => {
                const video = document.createElement("video");

                video.addEventListener('loadedmetadata', function onLoaded() {
                    const idx = GL.getNewId(videoElements);

                    video.removeEventListener('loadedmetadata', onLoaded);
                    videoElements[idx] = video;

                    if (callback) {{{ makeDynCall('vii', 'callback') }}}(idx, callbackArg);
                });

                video.srcObject = stream;                      
            }
        ).catch(_ => {
            if (callback) {{{ makeDynCall('vii', 'callback') }}}(0, callbackArg);
        })
    },
    siv3dOpenCamera__sig: "viiii",
    siv3dOpenCamera__deps: ["$videoElements"],

    siv3dCaptureVideoFrame: function(target, level, internalFormat, width, height, border, format, type, idx) {
        const video = videoElements[idx];
        GLctx.texImage2D(target, level, internalFormat, width, height, border, format, type, video);
    },
    siv3dCaptureVideoFrame__sig: "viiiiiiiii",
    siv3dCaptureVideoFrame__deps: ["$videoElements"],

    siv3dQueryVideoPlaybackedTime: function(idx) {
        const video = videoElements[idx];
        return video.currentTime;
    },
    siv3dQueryVideoPlaybackedTime__sig: "di",
    siv3dQueryVideoPlaybackedTime__deps: ["$videoElements"],

    siv3dPlayVideo: function(idx) {
        const video = videoElements[idx];
        video.play();
    },
    siv3dPlayVideo__sig: "vi",
    siv3dPlayVideo__deps: ["$videoElements"],

    siv3dStopVideo: function(idx) {
        const video = videoElements[idx];

        let stream = video.srcObject;
        let tracks = stream.getTracks();
      
        tracks.forEach(function(track) {
            track.stop();
        });
    },
    siv3dStopVideo__sig: "vi",
    siv3dStopVideo__deps: ["$videoElements"],

    siv3dDestroyVideo: function(idx) {
        _siv3dStopVideo(idx);
        delete videoElements[idx];
    },
    siv3dDestroyVideo__sig: "vi",
    siv3dDestroyVideo__deps: ["$videoElements"],

    //
    // User Action Emulation
    //
    $siv3dHasUserActionTriggered: false,
    $siv3dPendingUserActions: [],

    $siv3dTriggerUserAction: function() {
        for (let action of siv3dPendingUserActions) {
            action();
        }

        siv3dPendingUserActions.splice(0);
        siv3dHasUserActionTriggered = false;
    },
    $siv3dTriggerUserAction__deps: [ "$siv3dPendingUserActions" ],

    $siv3dRegisterUserAction: function(func) {
        siv3dPendingUserActions.push(func);
    },
    $siv3dRegisterUserAction__deps: [ "$siv3dPendingUserActions" ],

    $siv3dUserActionHookCallBack: function() {
        if (!siv3dHasUserActionTriggered) {
            setTimeout(siv3dTriggerUserAction, 30);
            siv3dHasUserActionTriggered = true;
        }
    },
    $siv3dUserActionHookCallBack__deps: [ "$siv3dHasUserActionTriggered", "$siv3dTriggerUserAction" ],

    siv3dStartUserActionHook: function() {
        Module["canvas"].addEventListener('touchstart', siv3dUserActionHookCallBack);
        Module["canvas"].addEventListener('mousedown', siv3dUserActionHookCallBack);
        window.addEventListener('keydown', siv3dUserActionHookCallBack);
    },
    siv3dStartUserActionHook__sig: "v",
    siv3dStartUserActionHook__deps: [ "$siv3dUserActionHookCallBack", "$siv3dHasUserActionTriggered" ],

    siv3dStopUserActionHook: function() {
        Module["canvas"].removeEventListener('touchstart', siv3dUserActionHookCallBack);
        Module["canvas"].removeEventListener('mousedown', siv3dUserActionHookCallBack);
        window.removeEventListener('keydown', siv3dUserActionHookCallBack);
    },
    siv3dStopUserActionHook__sig: "v",
    siv3dStopUserActionHook__deps: [ "$siv3dUserActionHookCallBack" ],

    //
    // Dialog Support
    //
    $siv3dInputElement: null,
    $siv3dDialogFileReader: null,
    $siv3dDownloadLink: null,

    siv3dInitDialog: function() {
        siv3dInputElement = document.createElement("input");
        siv3dInputElement.type = "file";

        siv3dDialogFileReader = new FileReader();

        siv3dSaveFileBuffer = new Uint8Array(16*1024 /* 16KB */)
        siv3dDownloadLink = document.createElement("a");

        TTY.register(FS.makedev(20, 0), { put_char: siv3dWriteSaveFileBuffer, flush: siv3dFlushSaveFileBuffer });
        FS.mkdev('/dev/save', FS.makedev(20, 0));
    },
    siv3dInitDialog__sig: "v",
    siv3dInitDialog__deps: [ "$siv3dInputElement", "$siv3dDialogFileReader", "$siv3dWriteSaveFileBuffer", "$siv3dFlushSaveFileBuffer", "$siv3dSaveFileBuffer", "$siv3dDownloadLink", "$TTY", "$FS" ],

    siv3dOpenDialog: function(filterStr, callback, futurePtr) {
        siv3dInputElement.accept = UTF8ToString(filterStr);
        siv3dInputElement.oninput = function(e) {
            const files = e.target.files;

            if (files.length < 1) {
                {{{ makeDynCall('vii', 'callback') }}}(0, futurePtr);
                return;
            }

            const file = files[0];
            const filePath = `/tmp/${file.name}`;

            siv3dDialogFileReader.addEventListener("load", function onLoaded() {
                FS.writeFile(filePath, new Uint8Array(siv3dDialogFileReader.result));

                const namePtr = allocate(intArrayFromString(filePath), 'i8', ALLOC_NORMAL);
                {{{ makeDynCall('vii', 'callback') }}}(namePtr, futurePtr);

                siv3dDialogFileReader.removeEventListener("load", onLoaded);
            });

            siv3dDialogFileReader.readAsArrayBuffer(file);         
        };

        siv3dRegisterUserAction(function() {
            siv3dInputElement.click();
        });
    },
    siv3dOpenDialog__sig: "vii",
    siv3dOpenDialog__deps: [ "$siv3dInputElement", "$siv3dDialogFileReader", "$siv3dRegisterUserAction", "$FS" ],

    $siv3dSaveFileBuffer: null, 
    $siv3dSaveFileBufferWritePos: 0,
    $siv3dDefaultSaveFileName: null,

    $siv3dWriteSaveFileBuffer: function(tty, chr) {       
        if (siv3dSaveFileBufferWritePos >= siv3dSaveFileBuffer.length) {
            const newBuffer = new Uint8Array(siv3dSaveFileBuffer.length * 2);
            newBuffer.set(siv3dSaveFileBuffer);
            siv3dSaveFileBuffer = newBuffer;
        }

        siv3dSaveFileBuffer[siv3dSaveFileBufferWritePos] = chr;
        siv3dSaveFileBufferWritePos++;
    },
    $siv3dWriteSaveFileBuffer__deps: [ "$siv3dSaveFileBuffer", "$siv3dSaveFileBufferWritePos" ], 
    $siv3dFlushSaveFileBuffer: function(tty) {
        if (siv3dSaveFileBufferWritePos == 0) {
            return;
        }

        const data = siv3dSaveFileBuffer.subarray(0, siv3dSaveFileBufferWritePos);
        const blob = new Blob([ data ], { type: "application/octet-stream" });

        siv3dDownloadLink.href = URL.createObjectURL(blob);
        siv3dDownloadLink.download = siv3dDefaultSaveFileName;

        siv3dRegisterUserAction(function() {
            siv3dDownloadLink.click();         
        });

        siv3dSaveFileBufferWritePos = 0;
    },
    $siv3dWriteSaveFileBuffer__deps: [ "$siv3dSaveFileBuffer", "$siv3dSaveFileBufferWritePos", "$siv3dRegisterUserAction", "$siv3dDefaultSaveFileName", "$siv3dDownloadLink" ], 

    siv3dSaveDialog: function(str) {
        siv3dDefaultSaveFileName = UTF8ToString(str);
        siv3dSaveFileBufferWritePos = 0;
    },
    siv3dSaveDialog__sig: "v",
    siv3dSaveDialog__deps: [ "$siv3dSaveFileBufferWritePos", "$siv3dDefaultSaveFileName" ],

    //
    // Clipboard
    //
    siv3dSetClipboardText: function(ctext) {
        const text = UTF8ToString(ctext);
        
        siv3dRegisterUserAction(function () {
            navigator.clipboard.writeText(text);
        });
    },
    siv3dSetClipboardText__sig: "vi",
    siv3dSetClipboardText__deps: [ "$siv3dRegisterUserAction" ],

    siv3dGetClipboardText: function(callback, promise) {
        siv3dRegisterUserAction(function () {
            navigator.clipboard.readText()
            .then(str => {
                const strPtr = allocate(intArrayFromString(str), 'i8', ALLOC_NORMAL);       
                {{{ makeDynCall('vii', 'callback') }}}(strPtr, promise);
                Module["_free"](strPtr);
            })
            .catch(e => {
                {{{ makeDynCall('vii', 'callback') }}}(0, promise);
            })
        });
        
    },
    siv3dGetClipboardText__sig: "vii",
    siv3dGetClipboardText__deps: [ "$siv3dRegisterUserAction" ],

    //
    // TextInput
    //
    $siv3dTextInputElement: null,

    siv3dInitTextInput: function() {
        const textInput = document.createElement("input");
        textInput.type = "text";
        textInput.style.position = "absolute";
        textInput.style.zIndex = -2;
        textInput.autocomplete = false;

        const maskDiv = document.createElement("div");
        maskDiv.style.background = "white";
        maskDiv.style.position = "absolute";
        maskDiv.style.width = "100%";
        maskDiv.style.height = "100%";
        maskDiv.style.zIndex = -1;

        /**
         * @type { HTMLCanvasElement }
         */
        const canvas = Module["canvas"];

        canvas.parentNode.prepend(textInput);
        canvas.parentNode.prepend(maskDiv);

        siv3dTextInputElement = textInput;
    },
    siv3dInitTextInput__sig: "v",
    siv3dInitTextInput__deps: [ "$siv3dTextInputElement" ],

    siv3dRegisterTextInputCallback: function(callback) {
        siv3dTextInputElement.addEventListener('input', function (e) {
            if (e.inputType == "insertText") {
                if (e.data) {
                    for (let i = 0; i < e.data.length; i++) {
                        const codePoint = e.data.charCodeAt(i);
                        {{{ makeDynCall('vi', 'callback') }}}(codePoint);
                    }
                }
            }    
        });
        siv3dTextInputElement.addEventListener('compositionend', function (e) {
            for (let i = 0; i < e.data.length; i++) {
                const codePoint = e.data.charCodeAt(i);
                {{{ makeDynCall('vi', 'callback') }}}(codePoint);
            }
        });
    },
    siv3dRegisterTextInputCallback__sig: "vi",
    siv3dRegisterTextInputCallback__deps: [ "$siv3dTextInputElement" ],

    siv3dRegisterTextInputMarkedCallback: function(callback) {
        siv3dTextInputElement.addEventListener('compositionupdate', function (e) {
            const strPtr = allocate(intArrayFromString(e.data), 'i8', ALLOC_NORMAL);
            {{{ makeDynCall('vi', 'callback') }}}(strPtr);
            Module["_free"](strPtr);
        })
        siv3dTextInputElement.addEventListener('compositionend', function (e) {
            {{{ makeDynCall('vi', 'callback') }}}(0);
        });
    },
    siv3dRegisterTextInputMarkedCallback__sig: "vi",
    siv3dRegisterTextInputMarkedCallback__deps: [ "$siv3dTextInputElement" ],

    siv3dRequestTextInputFocus: function(isFocusRequired) {
        const isFocusRequiredBool = isFocusRequired != 0;

        if (isFocusRequiredBool) {
            siv3dRegisterUserAction(function () {
                siv3dTextInputElement.value = ""
                siv3dTextInputElement.focus();
            });
        } else {
            siv3dRegisterUserAction(function () {
                siv3dTextInputElement.blur();
            });
        }
    },
    siv3dRequestTextInputFocus__sig: "vi",
    siv3dRequestTextInputFocus__deps: [ "$siv3dRegisterUserAction", "$siv3dTextInputElement" ],
})