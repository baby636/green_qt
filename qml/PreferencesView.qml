import Blockstream.Green.Core 0.1
import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.13

MainPage {
    id: self
    title: 'App Settings'

    header: MainPageHeader {
        contentItem: RowLayout {
            Label {
                text: self.title
                font.pixelSize: 24
                font.styleName: 'Medium'
            }
            HSpacer {}
            GButton {
                text: qsTrId('id_support')
                large: true
                highlighted: true
                onClicked: Qt.openUrlExternally(constants.supportUrl)
            }
        }
    }
    contentItem: ScrollView {
        id: scroll_view
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ColumnLayout {
            width: scroll_view.availableWidth
            spacing: 16
            MainPageSection {
                Layout.fillWidth: true
                title: qsTrId('General')
                contentItem: GridLayout {
                    columns: 3
                    columnSpacing: 16
                    rowSpacing: 8
                    Label {
                        text: qsTrId('Language')
                    }
                    ComboBox {
                        id: control
                        Layout.minimumWidth: 400
                        flat: true
                        model: languages
                        textRole: 'name'
                        valueRole: 'language'
                        currentIndex: {
                            for (let i = 0; i < languages.length; ++i) {
                                if (languages[i].language === Settings.language) return i;
                            }
                            return -1
                        }
                        onCurrentValueChanged: Settings.language = currentValue
                        font.capitalization: Font.Capitalize
                        delegate: ItemDelegate {
                            width: control.width
                            contentItem: Text {
                                text: modelData.name
                                color: 'white'
                                font: control.font
                                elide: Text.ElideRight
                                verticalAlignment: Text.AlignVCenter
                            }
                            highlighted: control.highlightedIndex === index
                        }
                    }
                    HSpacer {
                    }
                    Label {
                        text: 'Collapse Side Bar'
                    }
                    Switch {
                        checked: Settings.collapseSideBar
                        onCheckedChanged: Settings.collapseSideBar = checked
                    }
                    HSpacer {
                    }
                    Label {
                        text: qsTrId('Enable testnet')
                    }
                    Switch {
                        id: testnet_switch
                        checked: Settings.enableTestnet
                        onCheckedChanged: Settings.enableTestnet = checked
                    }
                    HSpacer {
                    }
                }
            }
            MainPageSection {
                Layout.fillWidth: true
                title: qsTrId('id_network')
                contentItem: GridLayout {
                    columns: 3
                    columnSpacing: 16
                    rowSpacing: 8
                    Label {
                        text: qsTrId('id_connect_through_a_proxy')
                    }
                    Switch {
                        id: proxy_switch
                        checked: Settings.useProxy
                        onCheckedChanged: Settings.useProxy = checked
                    }
                    HSpacer {
                    }
                    Label {
                        text: qsTrId('Proxy host')
                        enabled: proxy_switch.checked
                    }
                    TextField {
                        id: proxy_host_field
                        enabled: Settings.useProxy
                        Layout.alignment: Qt.AlignLeft
                        Layout.minimumWidth: 200
                        text: Settings.proxyHost
                        onTextChanged: Settings.proxyHost = text
                    }
                    HSpacer {
                    }
                    Label {
                        text: qsTrId('Proxy port')
                        enabled: proxy_switch.checked
                    }
                    TextField {
                        id: proxy_port_field
                        enabled: Settings.useProxy
                        Layout.alignment: Qt.AlignLeft
                        Layout.maximumWidth: 60
                        text: Settings.proxyPort
                        onTextChanged: Settings.proxyPort = text
                    }
                    HSpacer {
                    }
                    Label {
                        text: qsTrId('id_connect_with_tor')
                    }
                    Switch {
                        id: tor_switch
                        Layout.alignment: Qt.AlignLeft
                        checked: Settings.useTor
                        onCheckedChanged: Settings.useTor = checked
                    }
                    HSpacer {
                    }
                }
            }
        }
    }
}
