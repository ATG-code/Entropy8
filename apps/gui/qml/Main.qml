import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

// ═════════════════════════════════════════════════════════════════════════════
// Main Window – Keka-style compact dark archiver UI
// ═════════════════════════════════════════════════════════════════════════════
ApplicationWindow {
    id: root
    visible: true
    width: 420
    height: 640
    minimumWidth: 360
    minimumHeight: 480
    title: "Entropy8"
    color: theme.bg

    // ─── Language ──────────────────────────────────────────────────────────
    property string lang: "en"

    // ─── Theme ───────────────────────────────────────────────────────────
    QtObject {
        id: theme
        readonly property color bg:          "#1a1a1c"
        readonly property color surface:     "#232327"
        readonly property color panel:       "#2e2e33"
        readonly property color inputBg:     "#28282d"
        readonly property color border:      "#3a3a42"
        readonly property color text:        "#e6e6eb"
        readonly property color textDim:     "#74747e"
        readonly property color accent:      "#4090f5"
        readonly property color accentHover: "#59a5ff"
        readonly property color green:       "#40c87a"
        readonly property color red:         "#f56464"
        readonly property int   radius:      8
        readonly property string fontFamily: "Inter Tight"
    }

    // ─── Translations ────────────────────────────────────────────────────
    QtObject {
        id: tr
        readonly property bool isTr: root.lang === "tr"

        // Labels
        readonly property string method:      isTr ? "Metot:"  : "Method:"
        readonly property string split:        isTr ? "B\u00f6lme:"  : "Split:"
        readonly property string password:     isTr ? "\u015eifre:"  : "Password:"
        readonly property string repeat:       isTr ? "Tekrar:"  : "Repeat:"
        readonly property string splitHint:    isTr ? "\u00d6rnek: 5 MB" : "Example: 5 MB"

        // Slider ticks
        readonly property var methods: isTr
            ? ["Depola", "H\u0131zl\u0131", "Normal", "Yava\u015f"]
            : ["Store", "Fast", "Normal", "Slow"]

        // Encryption
        readonly property string encryptAvail:   isTr ? "\uD83D\uDD12 AES-256 \u015fifreleme / \u00e7\u00f6zme" : "\uD83D\uDD12 AES-256 encrypt / decrypt"
        readonly property string encryptDecOnly:  isTr ? "\uD83D\uDD13 \u015eifre sadece \u00e7\u00f6zme i\u00e7in kullan\u0131l\u0131r" : "\uD83D\uDD13 Password used for decryption only"

        // Checkboxes
        readonly property string encryptFilenames: isTr ? "Dosya adlar\u0131n\u0131 \u015fifrele"   : "Encrypt filenames"
        readonly property string solidArchive:     isTr ? "Tek par\u00e7a ar\u015fiv"               : "Solid archive"
        readonly property string selfExtracting:   isTr ? "Windows i\u00e7in kendinden a\u00e7\u0131lan ar\u015fiv" : "Self-extracting archive for Windows"
        readonly property string verifyIntegrity:  isTr ? "S\u0131k\u0131\u015ft\u0131rma b\u00fct\u00fcnl\u00fc\u011f\u00fcn\u00fc do\u011frula"     : "Verify compression integrity"
        readonly property string deleteAfter:      isTr ? "S\u0131k\u0131\u015ft\u0131rmadan sonra dosyalar\u0131 sil" : "Delete file(s) after compression"
        readonly property string archiveSeparately: isTr ? "\u00d6\u011feleri ayr\u0131 ayr\u0131 ar\u015fivle"       : "Archive items separately"

        // Drop / status
        readonly property string dropOverlay:  isTr ? "S\u0131k\u0131\u015ft\u0131rmak veya a\u00e7mak i\u00e7in b\u0131rak\u0131n" : "Drop files to compress or open"
        readonly property string dropHint:     isTr ? "Dosyalar\u0131 s\u00fcr\u00fckleyip b\u0131rak\u0131n"          : "Drop files here to compress or open"

        // Viewer
        readonly property string extractAll:   isTr ? "T\u00fcm\u00fcn\u00fc \u00c7\u0131kar" : "Extract All"
        readonly property string encrypted:    isTr ? "\uD83D\uDD12 \u015eifreli"             : "\uD83D\uDD12 Encrypted"
        readonly property string entries:      isTr ? "kay\u0131t"                             : "entries"
        readonly property string colName:      isTr ? "Ad"     : "Name"
        readonly property string colSize:      isTr ? "Boyut"  : "Size"
        readonly property string colCodec:     isTr ? "Kodek"  : "Codec"
        readonly property string extractTo:    isTr ? "\u00c7\u0131kar\u0131lacak Klas\u00f6r" : "Extract To"
    }

    // ─── Folder dialog for extraction ────────────────────────────────────
    FolderDialog {
        id: folderDialog
        title: tr.extractTo
        onAccepted: backend.extractAll(selectedFolder)
    }

    // ─── Drop area (full window) ─────────────────────────────────────────
    DropArea {
        id: dropArea
        anchors.fill: parent
        keys: ["text/uri-list"]
        onDropped: function(drop) {
            backend.handleDroppedUrls(drop.urls)
        }

        // Visual drop overlay
        Rectangle {
            id: dropOverlay
            anchors.fill: parent
            color: Qt.rgba(0.25, 0.56, 0.96, 0.12)
            border.color: theme.accent
            border.width: 2
            radius: theme.radius
            visible: dropArea.containsDrag
            z: 100

            Text {
                anchors.centerIn: parent
                text: tr.dropOverlay
                font.family: theme.fontFamily
                font.pixelSize: 16
                font.weight: Font.DemiBold
                color: theme.accent
            }
        }

        // ─── Main content ────────────────────────────────────────────────
        Flickable {
            anchors.fill: parent
            anchors.margins: 20
            contentHeight: mainColumn.height
            clip: true
            interactive: contentHeight > height

            ColumnLayout {
                id: mainColumn
                width: parent.width
                spacing: 6

                // ═══════════════════════════════════════════════════════════
                // Top row: language toggle (left) + format badge (right)
                // ═══════════════════════════════════════════════════════════
                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36

                    // Language toggle
                    Rectangle {
                        id: langBadge
                        anchors.left: parent.left
                        width: langRow.width + 16
                        height: 32
                        radius: 8
                        color: langMa.containsMouse ? theme.panel : theme.surface
                        border.color: theme.border
                        border.width: 1

                        Row {
                            id: langRow
                            anchors.centerIn: parent
                            spacing: 6

                            Text {
                                text: "\uD83C\uDF10"
                                font.pixelSize: 14
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            Text {
                                text: root.lang === "tr" ? "TR" : "EN"
                                font.family: theme.fontFamily
                                font.pixelSize: 12
                                font.weight: Font.DemiBold
                                color: theme.text
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }

                        MouseArea {
                            id: langMa
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.lang = (root.lang === "en") ? "tr" : "en"
                        }
                    }

                    // Format badge button
                    Rectangle {
                        id: formatBadge
                        anchors.right: parent.right
                        width: badgeText.width + 28
                        height: 32
                        radius: 8
                        color: backend.formatColor

                        Text {
                            id: badgeText
                            anchors.centerIn: parent
                            text: backend.formatName
                            font.family: theme.fontFamily
                            font.pixelSize: 13
                            font.weight: Font.Bold
                            color: "white"
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            hoverEnabled: true
                            onClicked: formatPopup.open()
                            onEntered: parent.opacity = 0.85
                            onExited: parent.opacity = 1.0
                        }
                    }
                }

                // ═══════════════════════════════════════════════════════════
                // Method slider
                // ═══════════════════════════════════════════════════════════
                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 70
                    Layout.topMargin: 4

                    Text {
                        id: methodLabel
                        anchors.left: parent.left
                        anchors.top: parent.top
                        text: tr.method
                        font.family: theme.fontFamily
                        font.pixelSize: 13
                        font.weight: Font.Medium
                        color: theme.textDim
                    }

                    Slider {
                        id: methodSlider
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: methodLabel.bottom
                        anchors.topMargin: 6
                        from: 0; to: 3; stepSize: 1
                        value: backend.methodIndex
                        onMoved: backend.methodIndex = value
                        snapMode: Slider.SnapAlways

                        background: Rectangle {
                            x: methodSlider.leftPadding
                            y: methodSlider.topPadding + methodSlider.availableHeight / 2 - height / 2
                            implicitWidth: 200
                            implicitHeight: 4
                            width: methodSlider.availableWidth
                            height: 4
                            radius: 2
                            color: theme.panel

                            Rectangle {
                                width: methodSlider.visualPosition * parent.width
                                height: parent.height
                                color: theme.accent
                                radius: 2
                            }
                        }

                        handle: Rectangle {
                            x: methodSlider.leftPadding + methodSlider.visualPosition * (methodSlider.availableWidth - width)
                            y: methodSlider.topPadding + methodSlider.availableHeight / 2 - height / 2
                            width: 18; height: 18
                            radius: 9
                            color: methodSlider.pressed ? theme.accentHover : theme.accent

                            Behavior on color { ColorAnimation { duration: 100 } }

                            HoverHandler { cursorShape: Qt.PointingHandCursor }
                        }
                    }

                    // Tick labels
                    Row {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: methodSlider.bottom
                        anchors.topMargin: 2
                        anchors.leftMargin: methodSlider.leftPadding
                        anchors.rightMargin: methodSlider.rightPadding

                        Repeater {
                            model: tr.methods
                            Item {
                                width: parent.width / 4
                                height: 16
                                Text {
                                    anchors.horizontalCenter: index === 0 ? undefined : (index === 3 ? undefined : parent.horizontalCenter)
                                    anchors.left: index === 0 ? parent.left : undefined
                                    anchors.right: index === 3 ? parent.right : undefined
                                    text: modelData
                                    font.family: theme.fontFamily
                                    font.pixelSize: 11
                                    font.weight: backend.methodIndex === index ? Font.DemiBold : Font.Normal
                                    color: backend.methodIndex === index ? theme.text : theme.textDim
                                    Behavior on color { ColorAnimation { duration: 150 } }
                                }
                            }
                        }
                    }
                }

                // ═══════════════════════════════════════════════════════════
                // Split field
                // ═══════════════════════════════════════════════════════════
                RowLayout {
                    Layout.fillWidth: true
                    Layout.topMargin: 4
                    spacing: 10

                    Text {
                        text: tr.split
                        Layout.preferredWidth: 72
                        font.family: theme.fontFamily
                        font.pixelSize: 13
                        font.weight: Font.Medium
                        color: theme.textDim
                    }

                    TextField {
                        id: splitField
                        Layout.fillWidth: true
                        placeholderText: tr.splitHint
                        text: backend.splitValue
                        onTextChanged: backend.splitValue = text
                        font.family: theme.fontFamily
                        font.pixelSize: 13
                        color: theme.text
                        placeholderTextColor: Qt.rgba(0.45, 0.45, 0.5, 0.6)

                        background: Rectangle {
                            implicitHeight: 32
                            radius: theme.radius
                            color: theme.inputBg
                            border.color: splitField.activeFocus ? theme.accent : theme.border
                            border.width: 1
                            Behavior on border.color { ColorAnimation { duration: 150 } }
                        }
                    }
                }

                // ═══════════════════════════════════════════════════════════
                // Encryption indicator
                // ═══════════════════════════════════════════════════════════
                Rectangle {
                    Layout.fillWidth: true
                    Layout.topMargin: 6
                    height: 24
                    radius: 6
                    color: backend.supportsEncryption
                           ? Qt.rgba(0.25, 0.56, 0.96, 0.08)
                           : Qt.rgba(1, 1, 1, 0.03)
                    border.color: backend.supportsEncryption
                                  ? Qt.rgba(0.25, 0.56, 0.96, 0.2)
                                  : theme.border
                    border.width: 1
                    visible: true

                    Text {
                        anchors.centerIn: parent
                        text: backend.supportsEncryption ? tr.encryptAvail : tr.encryptDecOnly
                        font.family: theme.fontFamily
                        font.pixelSize: 11
                        color: backend.supportsEncryption ? theme.accent : theme.textDim
                    }
                }

                // ═══════════════════════════════════════════════════════════
                // Password
                // ═══════════════════════════════════════════════════════════
                RowLayout {
                    Layout.fillWidth: true
                    Layout.topMargin: 6
                    spacing: 10

                    Text {
                        text: tr.password
                        Layout.preferredWidth: 72
                        font.family: theme.fontFamily
                        font.pixelSize: 13
                        font.weight: Font.Medium
                        color: theme.textDim
                    }

                    TextField {
                        id: pwField
                        Layout.fillWidth: true
                        echoMode: pwEye.checked ? TextInput.Normal : TextInput.Password
                        text: backend.password
                        onTextChanged: backend.password = text
                        font.family: theme.fontFamily
                        font.pixelSize: 13
                        color: theme.text

                        background: Rectangle {
                            implicitHeight: 32
                            radius: theme.radius
                            color: theme.inputBg
                            border.color: pwField.activeFocus ? theme.accent : theme.border
                            border.width: 1
                            Behavior on border.color { ColorAnimation { duration: 150 } }
                        }
                    }

                    AbstractButton {
                        id: pwEye
                        checkable: true
                        hoverEnabled: true
                        Layout.preferredWidth: 30
                        Layout.preferredHeight: 32

                        contentItem: Text {
                            text: pwEye.checked ? "\u25C9" : "\u25CE"
                            font.pixelSize: 16
                            color: pwEye.hovered ? theme.text : theme.textDim
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            Behavior on color { ColorAnimation { duration: 100 } }
                        }

                        background: Rectangle {
                            radius: theme.radius
                            color: pwEye.hovered ? theme.panel : "transparent"
                        }

                        HoverHandler { cursorShape: Qt.PointingHandCursor }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Text {
                        text: tr.repeat
                        Layout.preferredWidth: 72
                        font.family: theme.fontFamily
                        font.pixelSize: 13
                        font.weight: Font.Medium
                        color: theme.textDim
                    }

                    TextField {
                        id: pw2Field
                        Layout.fillWidth: true
                        echoMode: pw2Eye.checked ? TextInput.Normal : TextInput.Password
                        text: backend.passwordRepeat
                        onTextChanged: backend.passwordRepeat = text
                        font.family: theme.fontFamily
                        font.pixelSize: 13
                        color: theme.text

                        background: Rectangle {
                            implicitHeight: 32
                            radius: theme.radius
                            color: theme.inputBg
                            border.color: pw2Field.activeFocus ? theme.accent : theme.border
                            border.width: 1
                            Behavior on border.color { ColorAnimation { duration: 150 } }
                        }
                    }

                    AbstractButton {
                        id: pw2Eye
                        checkable: true
                        hoverEnabled: true
                        Layout.preferredWidth: 30
                        Layout.preferredHeight: 32

                        contentItem: Text {
                            text: pw2Eye.checked ? "\u25C9" : "\u25CE"
                            font.pixelSize: 16
                            color: pw2Eye.hovered ? theme.text : theme.textDim
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            Behavior on color { ColorAnimation { duration: 100 } }
                        }

                        background: Rectangle {
                            radius: theme.radius
                            color: pw2Eye.hovered ? theme.panel : "transparent"
                        }

                        HoverHandler { cursorShape: Qt.PointingHandCursor }
                    }
                }

                // ═══════════════════════════════════════════════════════════
                // Checkboxes – group 1
                // ═══════════════════════════════════════════════════════════
                Item { Layout.preferredHeight: 6 }

                DarkCheckBox {
                    text: tr.encryptFilenames
                    checked: backend.encryptFilenames
                    onCheckedChanged: backend.encryptFilenames = checked
                }
                DarkCheckBox {
                    text: tr.solidArchive
                    checked: backend.solidArchive
                    onCheckedChanged: backend.solidArchive = checked
                }
                DarkCheckBox {
                    text: tr.selfExtracting
                    checked: backend.selfExtracting
                    onCheckedChanged: backend.selfExtracting = checked
                }

                // Separator
                Rectangle {
                    Layout.fillWidth: true
                    Layout.topMargin: 8
                    Layout.bottomMargin: 8
                    height: 1
                    color: theme.border
                    opacity: 0.5
                }

                // ═══════════════════════════════════════════════════════════
                // Checkboxes – group 2
                // ═══════════════════════════════════════════════════════════
                DarkCheckBox {
                    text: tr.verifyIntegrity
                    checked: backend.verifyIntegrity
                    onCheckedChanged: backend.verifyIntegrity = checked
                }
                DarkCheckBox {
                    text: tr.deleteAfter
                    checked: backend.deleteAfter
                    onCheckedChanged: backend.deleteAfter = checked
                }
                DarkCheckBox {
                    text: tr.archiveSeparately
                    checked: backend.archiveSeparately
                    onCheckedChanged: backend.archiveSeparately = checked
                }

                // ═══════════════════════════════════════════════════════════
                // Status message
                // ═══════════════════════════════════════════════════════════
                Item {
                    Layout.fillWidth: true
                    Layout.topMargin: 12
                    Layout.preferredHeight: statusText.visible ? statusText.implicitHeight + 16 : 0
                    visible: backend.statusMessage !== ""

                    Rectangle {
                        anchors.fill: parent
                        radius: theme.radius
                        color: backend.statusError
                               ? Qt.rgba(0.96, 0.39, 0.39, 0.08)
                               : Qt.rgba(0.25, 0.78, 0.48, 0.08)
                        border.color: backend.statusError
                                      ? Qt.rgba(0.96, 0.39, 0.39, 0.25)
                                      : Qt.rgba(0.25, 0.78, 0.48, 0.25)
                        border.width: 1
                    }

                    Text {
                        id: statusText
                        anchors.fill: parent
                        anchors.margins: 8
                        text: backend.statusMessage
                        visible: backend.statusMessage !== ""
                        font.family: theme.fontFamily
                        font.pixelSize: 12
                        color: backend.statusError ? theme.red : theme.green
                        wrapMode: Text.WordWrap
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                // ═══════════════════════════════════════════════════════════
                // Spacer + drop hint
                // ═══════════════════════════════════════════════════════════
                Item { Layout.fillHeight: true; Layout.minimumHeight: 8 }

                Text {
                    Layout.fillWidth: true
                    Layout.bottomMargin: 4
                    text: tr.dropHint
                    font.family: theme.fontFamily
                    font.pixelSize: 12
                    color: theme.textDim
                    horizontalAlignment: Text.AlignHCenter
                    opacity: 0.6
                }
            }
        }
    }

    // ═════════════════════════════════════════════════════════════════════════
    // Format selection popup
    // ═════════════════════════════════════════════════════════════════════════
    Popup {
        id: formatPopup
        x: root.width - width - 20
        y: 58
        width: 180
        padding: 8
        modal: true
        dim: false
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        Overlay.modal: Rectangle {
            color: "transparent"
        }

        background: Rectangle {
            radius: 10
            color: "#222226"
            border.color: theme.border
            border.width: 1
        }

        contentItem: Column {
            spacing: 2

            Repeater {
                model: backend.formats

                Rectangle {
                    width: 164
                    height: 30
                    radius: 6
                    color: formatRowMa.containsMouse ? theme.panel : "transparent"

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 8
                        anchors.rightMargin: 8
                        spacing: 8

                        // Checkmark
                        Text {
                            text: backend.formatIndex === index ? "\u2713" : ""
                            Layout.preferredWidth: 16
                            font.pixelSize: 13
                            font.weight: Font.Bold
                            color: theme.green
                        }

                        // Color dot
                        Rectangle {
                            Layout.preferredWidth: 12
                            Layout.preferredHeight: 12
                            radius: 3
                            color: modelData.color
                        }

                        // Format name
                        Text {
                            Layout.fillWidth: true
                            text: modelData.name
                            font.family: theme.fontFamily
                            font.pixelSize: 13
                            font.weight: backend.formatIndex === index ? Font.DemiBold : Font.Normal
                            color: theme.text
                        }

                        // Lock icon (encryption support)
                        Text {
                            text: modelData.encrypts ? "\uD83D\uDD12" : ""
                            font.pixelSize: 11
                            color: theme.textDim
                            Layout.preferredWidth: 16
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }

                    MouseArea {
                        id: formatRowMa
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            backend.formatIndex = index
                            formatPopup.close()
                        }
                    }
                }
            }
        }
    }

    // ═════════════════════════════════════════════════════════════════════════
    // Archive Viewer window
    // ═════════════════════════════════════════════════════════════════════════
    Window {
        id: viewerWindow
        visible: backend.showViewer && backend.archiveOpen
        width: 560
        height: 400
        title: "Archive: " + backend.archivePath
        color: theme.bg

        onClosing: backend.showViewer = false

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 10

            // Toolbar
            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                Rectangle {
                    width: extractBtn.implicitWidth + 24
                    height: 32
                    radius: theme.radius
                    color: extractBtnMa.containsMouse ? Qt.lighter(theme.accent, 1.15) : theme.accent

                    Text {
                        id: extractBtn
                        anchors.centerIn: parent
                        text: tr.extractAll
                        font.family: theme.fontFamily
                        font.pixelSize: 13
                        font.weight: Font.DemiBold
                        color: "white"
                    }

                    MouseArea {
                        id: extractBtnMa
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: folderDialog.open()
                    }
                }

                // Encrypted badge
                Rectangle {
                    visible: backend.archiveEncrypted
                    width: encBadgeText.implicitWidth + 16
                    height: 24
                    radius: 6
                    color: Qt.rgba(0.25, 0.56, 0.96, 0.12)
                    border.color: Qt.rgba(0.25, 0.56, 0.96, 0.3)
                    border.width: 1

                    Text {
                        id: encBadgeText
                        anchors.centerIn: parent
                        text: tr.encrypted
                        font.family: theme.fontFamily
                        font.pixelSize: 11
                        font.weight: Font.DemiBold
                        color: theme.accent
                    }
                }

                Text {
                    text: backend.entryCount + " " + tr.entries + "  |  " + backend.totalSize
                    font.family: theme.fontFamily
                    font.pixelSize: 12
                    color: theme.textDim
                }

                Item { Layout.fillWidth: true }
            }

            // Header
            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: theme.border
            }

            Rectangle {
                Layout.fillWidth: true
                height: 28
                color: theme.surface
                radius: 4

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 12
                    anchors.rightMargin: 12
                    spacing: 0

                    Text {
                        Layout.fillWidth: true
                        Layout.preferredWidth: 3
                        text: tr.colName
                        font.family: theme.fontFamily
                        font.pixelSize: 11
                        font.weight: Font.DemiBold
                        color: theme.textDim
                    }
                    Text {
                        Layout.fillWidth: true
                        Layout.preferredWidth: 1
                        text: tr.colSize
                        font.family: theme.fontFamily
                        font.pixelSize: 11
                        font.weight: Font.DemiBold
                        color: theme.textDim
                    }
                    Text {
                        Layout.fillWidth: true
                        Layout.preferredWidth: 0.8
                        text: tr.colCodec
                        font.family: theme.fontFamily
                        font.pixelSize: 11
                        font.weight: Font.DemiBold
                        color: theme.textDim
                    }
                }
            }

            // Entry list
            ListView {
                id: entryList
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: backend.entries
                spacing: 1

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded

                    contentItem: Rectangle {
                        implicitWidth: 6
                        radius: 3
                        color: theme.panel
                        opacity: parent.active ? 1.0 : 0.0
                        Behavior on opacity { NumberAnimation { duration: 200 } }
                    }
                }

                delegate: Rectangle {
                    width: entryList.width
                    height: 28
                    radius: 4
                    color: index % 2 === 0 ? "transparent" : Qt.rgba(1, 1, 1, 0.015)

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 12
                        spacing: 0

                        Text {
                            Layout.fillWidth: true
                            Layout.preferredWidth: 3
                            text: modelData.path
                            font.family: theme.fontFamily
                            font.pixelSize: 12
                            color: theme.text
                            elide: Text.ElideMiddle
                        }
                        Text {
                            Layout.fillWidth: true
                            Layout.preferredWidth: 1
                            text: modelData.sizeStr
                            font.family: theme.fontFamily
                            font.pixelSize: 12
                            color: theme.text
                        }
                        Text {
                            Layout.fillWidth: true
                            Layout.preferredWidth: 0.8
                            text: modelData.codecName
                            font.family: theme.fontFamily
                            font.pixelSize: 12
                            color: theme.textDim
                        }
                    }
                }
            }
        }
    }

    // ═════════════════════════════════════════════════════════════════════════
    // DarkCheckBox component
    // ═════════════════════════════════════════════════════════════════════════
    component DarkCheckBox: CheckBox {
        id: cb
        Layout.fillWidth: true
        spacing: 8
        hoverEnabled: true

        font.family: theme.fontFamily
        font.pixelSize: 13

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: cb.toggle()
        }

        indicator: Rectangle {
            implicitWidth: 18
            implicitHeight: 18
            x: cb.leftPadding
            y: parent.height / 2 - height / 2
            radius: 5
            color: cb.checked ? theme.green : theme.inputBg
            border.color: cb.checked ? theme.green : theme.border
            border.width: 1

            Behavior on color { ColorAnimation { duration: 150 } }
            Behavior on border.color { ColorAnimation { duration: 150 } }

            Text {
                anchors.centerIn: parent
                text: "\u2713"
                font.pixelSize: 12
                font.weight: Font.Bold
                color: "white"
                visible: cb.checked
            }
        }

        contentItem: Text {
            text: cb.text
            font: cb.font
            color: theme.text
            verticalAlignment: Text.AlignVCenter
            leftPadding: cb.indicator.width + cb.spacing
        }
    }
}
