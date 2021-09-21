# Rainbow.kak (C) 2021 Alessandro Manca. MIT License.
# Rainbow.kak (C) 2020 Landon Meernik. MIT License.

# Declare a range spec set.
# This holds the actual positions we want to highlight and their colors
declare-option -hidden range-specs rainbower
declare-option -hidden str-list window_range
declare-option -hidden str kak_rainbower_source %sh{ echo "${kak_source%/*}" }
declare-option -hidden int rainbower_last_timestamp -1
# Rainbow colors
declare-option str-list rainbower_colors
# colors from https://github.com/absop/RainbowBrackets
set-option global rainbower_colors rgb:FF6A00 rgb:FFD800 rgb:00FF00 rgb:0094FF rgb:0041FF rgb:7D00E5
declare-option str-list background_rainbower_colors
set-option global background_rainbower_colors rgb:331500 rgb:332200 rgb:003300 rgb:001833 rgb:000533 rgb:100021
declare-option int rainbower_mode
set-option global rainbower_mode 1

declare-option str rainbower_check_templates
set-option global rainbower_check_templates "n"
declare-option str rainbower_check_pound_ifs
set-option global rainbower_check_pound_ifs "Y"

define-command rainbower-enable-window -docstring "enable rainbow parentheses for this window" %{
    hook -group rainbower window NormalIdle .* %{
        %sh{
            if [ ${kak_opt_rainbower_last_timestamp} -eq ${kak_timestamp} ]; then
                echo rainbower-full-view
            else
                echo nop
            fi
        }
        set-option buffer rainbower_last_timestamp %val{timestamp}
    }
    hook -group rainbower window InsertIdle .* %{ rainbower-view }
    add-highlighter window/rainbower ranges rainbower
    rainbower-full-view
    set-option buffer rainbower_last_timestamp %val{timestamp}
}

define-command rainbower-disable-window -docstring "disable rainbow parentheses for this window" %{
    remove-hooks window rainbower
    remove-highlighter window/rainbower
}

define-command rainbower-compile %{
    evaluate-commands %sh{
        c++ ${kak_opt_kak_rainbower_source}/rainbower.cpp -o ${kak_opt_kak_rainbower_source}/rainbower
    }
}

# Does rainbow parens on the current view
define-command -hidden rainbower-view %{
    evaluate-commands -draft -save-regs ^ %{
        try %{
            set-option window window_range %val{window_range}
            execute-keys -save-regs _ ' ;Z<ret>' # save original main selection in ^ reg
            evaluate-commands -save-regs '|' %{
                execute-keys -draft '%<a-|>${kak_opt_kak_rainbower_source}/rainbower ${kak_client} "${kak_timestamp}" ${kak_opt_rainbower_mode} $(echo $kak_reg_caret | cut -d" " -f2) $(echo $kak_opt_window_range | cut -d " " --output-delimiter="." -f1-2) $(echo $kak_opt_window_range | cut -d " " --output-delimiter="." -f3-4) $kak_opt_filetype "$kak_opt_rainbower_check_templates" "$kak_opt_rainbower_check_pound_ifs" $kak_opt_rainbower_colors ! $kak_opt_background_rainbower_colors | kak -p "${kak_session}"<ret>'
            }
        }
    }
}

define-command -hidden rainbower-full-view %{
    evaluate-commands -draft -save-regs ^ %{
        try %{
            execute-keys -save-regs _ ' ;Z<ret>' # save original main selection in ^ reg
            evaluate-commands -save-regs '|' %{
                execute-keys -draft '%<a-|>${kak_opt_kak_rainbower_source}/rainbower ${kak_client} "${kak_timestamp}" ${kak_opt_rainbower_mode} $(echo $kak_reg_caret | cut -d" " -f2) 0.0 9999999.9999999 $kak_opt_filetype "$kak_opt_rainbower_check_templates" "$kak_opt_rainbower_check_pound_ifs" $kak_opt_rainbower_colors ! $kak_opt_background_rainbower_colors | kak -p "${kak_session}"<ret>'
            }
        }
    }
}


