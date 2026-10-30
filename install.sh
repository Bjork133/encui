#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LIB_DIR="$SCRIPT_DIR/lib"

source "$LIB_DIR/i18n.sh"
source "$LIB_DIR/ui.sh"
source "$LIB_DIR/crypto.sh"

SELECTED_ALGO=""
MODE="encrypt"
PUBLIC_KEY=""
PRIVATE_KEY=""
METHOD_KEY=""
SHIFT=""
SINGLE_KEY=""
AEAD_KEY=""
AEAD_NONCE=""
DATA=""
OUTPUT_PATH="output.bin"
RESULT_HEX=""

main() {
    # Баннер показываем один раз
    draw_banner
    sleep 1

    while true; do
        clear
        draw_banner
        menu_select "$MSG_MENU_HEADER" \
            "$MSG_MENU_1" \
            "$MSG_MENU_2  [$MODE]" \
            "$MSG_MENU_3" \
            "$MSG_MENU_4" \
            "$MSG_MENU_5" \
            "$MSG_MENU_6" \
            "$MSG_MENU_7" \
            "$MSG_MENU_Q"

        case "$MENU_RESULT" in
            0) select_algorithm ;;
            1) toggle_mode ;;
            2) input_params ;;
            3) input_data ;;
            4) run_crypto ;;
            5) save_result ;;
            6) draw_help; read -r _ ;;
            7) draw_goodbye; exit 0 ;;
            -1) draw_goodbye; exit 0 ;;
        esac
    done
}

main
