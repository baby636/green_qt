import Blockstream.Green 0.1
import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.14
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.12
import QtQml 2.15

MainPage {
    required property Wallet wallet
    readonly property string location: `/${wallet.network.key}/${wallet.id}`
    readonly property Account currentAccount: accounts_list.currentAccount
    readonly property bool fiatRateAvailable: formatFiat(0, false) !== 'n/a'

    function parseAmount(amount) {
        const unit = wallet.settings.unit;
        return wallet.parseAmount(amount, unit);
    }

    function formatAmount(amount, include_ticker = true) {
        const unit = wallet.settings.unit;
        return wallet.formatAmount(amount || 0, include_ticker, unit);
    }

    function formatFiat(sats, include_ticker = true) {
        const pricing = wallet.settings.pricing;
        const { fiat, fiat_currency } = wallet.convert({ satoshi: sats });
        return (fiat === null ? 'n/a' : Number(fiat).toLocaleString(Qt.locale(), 'f', 2)) + (include_ticker ? ' ' + fiat_currency : '');
    }

    function parseFiat(fiat) {
        fiat = fiat.trim().replace(/,/, '.');
        return fiat === '' ? 0 : wallet.convert({ fiat }).satoshi;
    }

    function transactionConfirmations(transaction) {
        if (transaction.data.block_height === 0) return 0;
        return 1 + transaction.account.wallet.events.block.block_height - transaction.data.block_height;
    }

    function transactionStatus(confirmations) {
        if (confirmations === 0) return qsTrId('id_unconfirmed');
        if (!wallet.network.liquid && confirmations < 6) return qsTrId('id_d6_confirmations').arg(confirmations);
        return qsTrId('id_completed');
    }

    FeeEstimates {
        id: fee_estimates
        wallet: self.wallet
    }

    DialogLoader {
        id: settings_dialog
        property string location: `${self.location}/settings`
        property bool enabled: {
            if (self.wallet.watchOnly) return false
            if (self.wallet.network.electrum) {
                return true
            }
            return !!self.wallet.settings.pricing && !!self.wallet.config.limits
        }
        active: settings_dialog.enabled && navigation.location === settings_dialog.location
        dialog: WalletSettingsDialog {
            parent: window.Overlay.overlay
            wallet: self.wallet
            onRejected: navigation.go(self.location)
        }
    }
    id: self
    leftPadding: 0
    rightPadding: 0
    header: WalletViewHeader {
        id: wallet_view_header
        currentAccount: self.currentAccount
        wallet: self.wallet
        onViewSelected: stack_view.currentItem.currentView = viewIndex
    }

    Drawer {
        id: notifications_drawer
        interactive: position > 0
        height: parent.height
        width: 320
        edge: Qt.RightEdge
        Overlay.modal: Rectangle {
            color: "#70000000"
        }
        ColumnLayout {
            width: 320
            spacing: 8
            Label {
                visible: !fiatRateAvailable
                text: qsTrId('id_your_favourite_exchange_rate_is')
                padding: 8
                leftPadding: 40
                wrapMode: Label.WordWrap
                Layout.fillWidth: true
                background: Rectangle {
                    color: 'white'
                    opacity: 0.05
                }
            }
            Label {
                visible: wallet.events && !!wallet.events.twofactor_reset && wallet.events.twofactor_reset.is_active
                padding: 8
                leftPadding: 40
                wrapMode: Label.WordWrap
                Layout.fillWidth: true
                text: {
                    const data = wallet.events.twofactor_reset
                    if (!data) return ''
                    if (data.is_disputed) {
                        return qsTrId('id_warning_wallet_locked_by')
                    }
                    if (data.is_active) {
                        console.assert(data.days_remaining > 0)
                        return qsTrId('id_your_wallet_is_locked_for_a').arg(data.days_remaining)
                    }
                    return ''
                }
                Image {
                    y: 8
                    x: 8
                    source: 'qrc:/svg/twofactor.svg'
                }
                background: Rectangle {
                    color: 'white'
                    opacity: 0.05
                }
            }
        }
    }


    Component {
        id: account_view_component
        AccountView {}
    }

    property var account_views: ({})
    function switchToAccount(account) {
        if (account) {
            let account_view = account_views[account]
            if (!account_view) {
                account_view = account_view_component.createObject(null, { account })
                account_views[account] = account_view
            }
            if (stack_view.currentItem === account_view) return;
            stack_view.replace(account_view, StackView.Immediate)
            wallet_view_header.currentView = account_view.currentView
        } else {
            stack_view.replace(stack_view.initialItem, StackView.Immediate)
        }
    }

    contentItem: SplitView {
        focusPolicy: Qt.ClickFocus
        handle: Item {
            implicitWidth: 4
            implicitHeight: parent.height
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            SplitView.minimumWidth: wallet_view_header.showAccounts ? 350 :0
            clip: true
            color: constants.c900

            Behavior on SplitView.minimumWidth {
                NumberAnimation { easing.type: Easing.OutCubic; duration: 150 }
            }

            AccountListView {
                id: accounts_list
                anchors.fill: parent
                anchors.margins: constants.p3
                anchors.bottomMargin: 0
                onClicked: switchToAccount(currentAccount)
                onCurrentAccountChanged: switchToAccount(currentAccount)
            }
        }

        Rectangle {
            SplitView.fillWidth: true
            SplitView.minimumWidth: self.width / 2
            color: constants.c900

            StackView {
                id: stack_view
                anchors.fill: parent
                anchors.margins: constants.p3
                anchors.bottomMargin: 0
                focusPolicy: Qt.ClickFocus
                initialItem: Item {}
                clip: true
            }
        }
    }

    Component {
        id: bump_fee_dialog
        BumpFeeDialog {
        }
    }

    SystemMessageDialog {
        id: system_message_dialog
        property bool alreadyOpened: false
        wallet: self.wallet
        visible: shouldOpen && !alreadyOpened && self.match
        onVisibleChanged: {
            if (!visible) {
                Qt.callLater(function () { system_message_dialog.alreadyOpened = true })
            }
        }
    }
}
