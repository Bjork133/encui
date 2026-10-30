
CLI_BIN="${CLI_BIN:-crypto-cli}"

select_algorithm() {
    menu_select "Выбор алгоритма" \
        "Conway-AES (public/private/method)" \
        "Caesar (shift)" \
        "Vigenere (key)" \
        "Vernam / OTP (key >= data)" \
        "AES-256-GCM (key+nonce hex)" \
        "ChaCha20-Poly1305 (key+nonce hex)"
}

CLI_BIN="${CLI_BIN:-crypto-cli}"

select_algorithm() {
    menu_select "Выбор алгоритма" \
        "Conway-AES (public/private/method)" \
        "Caesar (shift)" \
        "Vigenere (key)" \
        "Vernam / OTP (key >= data)" \
        "AES-256-GCM (key+nonce hex)" \
        "ChaCha20-Poly1305 (key+nonce hex)"

    case "$MENU_RESULT" in
        0) SELECTED_ALGO="conway-aes" ;;
        1) SELECTED_ALGO="caesar" ;;
        2) SELECTED_ALGO="vigenere" ;;
        3) SELECTED_ALGO="vernam" ;;
        4) SELECTED_ALGO="aes-gcm" ;;
        5) SELECTED_ALGO="chacha20poly1305" ;;
        -1) return ;;
    esac
    draw_status "Выбран алгоритм: $SELECTED_ALGO"
}

toggle_mode() {
    if [ "$MODE" = "encrypt" ]; then
        MODE="decrypt"
    else
        MODE="encrypt"
    fi
    draw_status "Режим: $MODE"
}

input_params() {
    if [ -z "$SELECTED_ALGO" ]; then
        draw_error "Сначала выберите алгоритм"
        return
    fi
    case "$SELECTED_ALGO" in
        conway-aes)
            read -rp "Public part: " PUBLIC_KEY
            read -rp "Private part: " PRIVATE_KEY
            read -rp "Method key: " METHOD_KEY
            ;;
        caesar)
            read -rp "Shift (0..25): " SHIFT
            ;;
        vigenere)
            read -rp "Key (letters): " SINGLE_KEY
            ;;
        vernam)
            read -rp "Key (>= data length): " SINGLE_KEY
            ;;
        aes-gcm|chacha20poly1305)
            read -rp "Key (64 hex chars = 32 bytes): " AEAD_KEY
            read -rp "Nonce (24 hex chars = 12 bytes): " AEAD_NONCE
            ;;
    esac
    draw_status "Параметры сохранены"
}

input_data() {
    read -rp "Данные: " DATA
    draw_status "Данные сохранены (${#DATA} байт)"
}

run_crypto() {
    if [ -z "$SELECTED_ALGO" ]; then
        draw_error "Сначала выберите алгоритм"
        return
    fi

    local args=(--algo "$SELECTED_ALGO" --mode "$MODE")

    case "$SELECTED_ALGO" in
        conway-aes)
            if [ -z "$METHOD_KEY" ]; then
                draw_error "Method key пуст"
                return
            fi
            args+=(--public "$PUBLIC_KEY" --private "$PRIVATE_KEY" --method "$METHOD_KEY" --data "$DATA")
            ;;
        caesar)
            if [ -z "$SHIFT" ]; then
                draw_error "Shift пуст"
                return
            fi
            args+=(--shift "$SHIFT" --data "$DATA")
            ;;
        vigenere|vernam)
            if [ -z "$SINGLE_KEY" ]; then
                draw_error "Key пуст"
                return
            fi
            args+=(--key "$SINGLE_KEY" --data "$DATA")
            ;;
        aes-gcm|chacha20poly1305)
            if [ -z "$AEAD_KEY" ] || [ -z "$AEAD_NONCE" ]; then
                draw_error "Key/Nonce пусты"
                return
            fi
            args+=(--key "$AEAD_KEY" --nonce "$AEAD_NONCE")
            if [ "$MODE" = "decrypt" ]; then
                args+=(--data-hex --data "$DATA")
            else
                args+=(--data "$DATA")
            fi
            ;;
    esac

    echo
    printf "${C_BOLD}Выполнение (%s)${C_RESET}\n" "$MODE"
    for i in 1 2 3 4 5; do
        draw_progress "$i" 5
        sleep 0.08
    done
    echo

    local result
    result=$("$CLI_BIN" "${args[@]}" 2>&1) || {
        draw_error "Ошибка: $result"
        return
    }
    RESULT_HEX="$result"
    draw_status "Готово: $RESULT_HEX"
}

save_result() {
    if [ -z "$RESULT_HEX" ]; then
        draw_error "Нечего сохранять — сначала выполните операцию"
        return
    fi
    read -rp "Путь для сохранения [$OUTPUT_PATH]: " p
    [ -n "$p" ] && OUTPUT_PATH="$p"

    # Раскрываем ведущую ~ (и ~user) вручную
    case "$OUTPUT_PATH" in
        "~") OUTPUT_PATH="$HOME" ;;
        "~/"*) OUTPUT_PATH="$HOME/${OUTPUT_PATH#\~/}" ;;
        "~"*)
            # ~user/... — раскрываем через eval, аккуратно
            OUTPUT_PATH=$(eval printf '%s' "$OUTPUT_PATH")
            ;;
    esac

    printf "%s" "$RESULT_HEX" | xxd -r -p > "$OUTPUT_PATH"
    draw_status "Сохранено в $OUTPUT_PATH"
}
