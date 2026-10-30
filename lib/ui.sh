C_RESET="\033[0m"
C_BOLD="\033[1m"
C_CYAN="\033[36m"
C_GREEN="\033[32m"
C_YELLOW="\033[33m"
C_RED="\033[31m"
C_DIM="\033[2m"
C_INVERT="\033[7m"

draw_banner() {
    clear
    printf "${C_CYAN}${C_BOLD}"
    cat <<'EOF'
   ______                __        ______  __  __ ______
  / ____/______  ______  / /_____  /_  __/ / / /  _/ /
 / /   / ___/ / / / __ \/ __/ __ \  / / / / / // / / /
/ /___/ /  / /_/ / /_/ / /_/ /_/ / / / / /_/ // / / /
\____/_/   \__, / .___/\__/\____/ /_/  \____/___/_/
          /____/_/
EOF
    printf "${C_RESET}\n"
    printf "${C_DIM}%s${C_RESET}\n\n" "$MSG_SUBTITLE"
}

# Меню со стрелками. Результат в MENU_RESULT (0-based, -1 = отмена).
menu_select() {
    local title="$1"; shift
    local items=("$@")
    local count=${#items[@]}
    local selected=0

    tput civis 2>/dev/null || true
    printf "%s\n" "$title"

    while true; do
        for i in "${!items[@]}"; do
            if [ "$i" -eq "$selected" ]; then
                printf "${C_INVERT}  > %s ${C_RESET}\n" "${items[$i]}"
            else
                printf "    %s\n" "${items[$i]}"
            fi
        done

        IFS= read -rsn1 key
        case "$key" in
            $'\x1b')
                read -rsn2 -t 0.1 rest
                case "$rest" in
                    "[A") selected=$(( (selected - 1 + count) % count )) ;;
                    "[B") selected=$(( (selected + 1) % count )) ;;
                esac
                ;;
            "") break ;;
            q|Q) MENU_RESULT=-1; tput cnorm 2>/dev/null || true; return ;;
        esac

        for _ in $(seq 1 "$count"); do
            tput cuu1 2>/dev/null || printf "\033[1A"
        done
    done

    tput cnorm 2>/dev/null || true
    MENU_RESULT=$selected
}

draw_status() { printf "${C_GREEN}[+]${C_RESET} %s\n" "$1"; }
draw_error()  { printf "${C_RED}[!]${C_RESET} %s\n" "$1"; }

draw_progress() {
    local current=$1 total=$2 width=40
    local filled=$(( current * width / total ))
    local empty=$(( width - filled ))
    printf "\r["
    printf "%${filled}s" | tr ' ' '#'
    printf "%${empty}s" | tr ' ' '-'
    printf "] %d/%d" "$current" "$total"
}

draw_goodbye() {
    printf "\n${C_CYAN}%s${C_RESET}\n" "$MSG_GOODBYE"
}

draw_help() {
    clear
    printf "${C_BOLD}Помощь${C_RESET}\n"
    echo "  Стрелки ↑/↓ — выбор, Enter — подтвердить, q — отмена"
    echo "  1 — алгоритм: conway-aes, caesar, vigenere, vernam, aes-gcm, chacha20poly1305"
    echo "  2 — encrypt/decrypt"
    echo "  3 — ввести параметры для выбранного алгоритма"
    echo "  4 — данные (текст; для decrypt AEAD — hex)"
    echo "  5 — выполнить"
    echo "  6 — сохранить hex-результат в файл"
    echo "  q — выход"
    echo
    printf "Enter — назад..."
}
