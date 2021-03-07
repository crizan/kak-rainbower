# Rainbow.kak (C) 2021 Alessandro Manca. MIT License.
# Rainbow.kak (C) 2020 Landon Meernik. MIT License.

# Declare a range spec set.
# This holds the actual positions we want to highlight and their colors
declare-option -hidden range-specs rainbow
declare-option -hidden str-list window_range
declare-option -hidden str kak_rainbower_source %sh{ echo "${kak_source%/*}" }
# Rainbow colors
declare-option str-list rainbow_colors
# colors from https://github.com/absop/RainbowBrackets
set-option global rainbow_colors rgb:FF6A00 rgb:FFD800 rgb:00FF00 rgb:0094FF rgb:0041FF rgb:7D00E5
declare-option str-list background_rainbow_colors
set-option global background_rainbow_colors rgb:331500 rgb:332200 rgb:003300 rgb:001833 rgb:000533 rgb:100021
declare-option int rainbow_mode
set-option global rainbow_mode 1

define-command rainbow-enable-window -docstring "enable rainbow parentheses for this window" %{
    hook -group rainbow window NormalIdle .* %{ rainbow-view }
    hook -group rainbow window InsertIdle .* %{ rainbow-view }
    add-highlighter window/rainbow ranges rainbow
}

define-command rainbow-disable-window -docstring "disable rainbow parentheses for this window" %{
    remove-hooks window rainbow
    remove-highlighter window/rainbow
}

define-command rainbower-compile %{
    evaluate-commands %sh{
        c++ ${kak_opt_kak_rainbower_source}/rainbower.cpp -o ${kak_opt_kak_rainbower_source}/rainbower
    }
}

# Does rainbow parens on the current view
define-command -hidden rainbow-view %{
    evaluate-commands -draft -save-regs ^ %{
        try %{
            set-option window window_range %val{window_range}
            execute-keys -save-regs _ ' ;Z<ret>' # save original main selection in ^ reg
            evaluate-commands -save-regs '|' %{
                execute-keys -draft '%<a-|>${kak_opt_kak_rainbower_source}/rainbower ${kak_client} "${kak_timestamp}" ${kak_opt_rainbow_mode} $(echo $kak_reg_caret | cut -d" " -f2) $(echo $kak_opt_window_range | cut -d " " --output-delimiter="." -f1-2) $(echo $kak_opt_window_range | cut -d " " --output-delimiter="." -f3-4) $kak_opt_filetype $kak_opt_rainbow_colors ! $kak_opt_background_rainbow_colors | kak -p "${kak_session}"<ret>'
            }
        }
    }
}

