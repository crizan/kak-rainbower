/*
 * MIT License

 * Copyright (c) 2021 Alessandro Manca

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct IntPair
{
    int a, b;
};

int ParseInt(const char *c, int *num_chars)
{
    int number = 0;
    if(num_chars)
    {
        *num_chars = 0;
    }

    while(*c >= '0' && *c <= '9')
    {
        number *= 10;
        number += *c - 48;
        ++c;
        if(num_chars)
        {
            ++(*num_chars);
        }
    }

    return number;
}

IntPair ParsePair(const char *c)
{
    IntPair p = {};

    int num_chars;
    p.a = ParseInt(c, &num_chars);
    c += num_chars;
    if(*c != '\0')
    {
        c++;
    }
    p.b = ParseInt(c, NULL);

    return p;
}

struct CharPosition
{
    IntPair pair;
    char c;
    int level;
};

#define BLOCK_SIZE 100

struct CharPositionVector
{
    CharPosition *array;
    int len;
    int size;
};

void Insert(CharPositionVector *vector, CharPosition elem)
{
    if(vector->len == vector->size)
    {
        int alloc_size = sizeof(CharPosition) * (vector->size + BLOCK_SIZE);
        vector->array = (CharPosition *)realloc(vector->array, alloc_size);
        vector->size += BLOCK_SIZE;
    }

    vector->array[vector->len] = elem;
    vector->len++;
}

struct RainbowStack
{
    CharPosition data;
    struct RainbowStack *previous;
};

RainbowStack *PushCharPosition(RainbowStack *s, CharPosition data)
{
    RainbowStack *new_element = (RainbowStack *)malloc(sizeof(RainbowStack));

    if(new_element)
    {
        new_element->data = data;
        new_element->previous = s;
    }

    return new_element;
}

RainbowStack *PopCharPosition(RainbowStack *s)
{
    if(s)
    {
        RainbowStack *previous = s->previous;

        free(s);

        return previous;
    }

    return NULL;
}

bool IsMaxPair(IntPair pair_a, IntPair pair_b)
{
    if(pair_a.a > pair_b.a)
    {
        return true;
    }
    else if(pair_a.a < pair_b.a)
    {
        return false;
    }
    else
    {
        if(pair_a.b >= pair_b.b)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool IsMinPair(IntPair pair_a, IntPair pair_b)
{
    if(pair_a.a < pair_b.a)
    {
        return true;
    }
    else if(pair_a.a > pair_b.a)
    {
        return false;
    }
    else
    {
        if(pair_a.b <= pair_b.b)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool IsRangeVisible(IntPair pair_a, IntPair pair_b, IntPair bound_a, IntPair bound_b)
{
    return ((IsMaxPair(pair_a, bound_a) && IsMinPair(pair_a, bound_b)) ||
            (IsMaxPair(pair_b, bound_a) && IsMinPair(pair_b, bound_b)) ||
            (IsMinPair(pair_a, bound_a) && IsMaxPair(pair_b, bound_b)));
}

#define UTF_8_SINGLE_BYTE_XOR_MASK 0b00000000
#define UTF_8_SINGLE_BYTE_AND_MASK 0b10000000
#define UTF_8_DOUBLE_BYTE_XOR_MASK 0b11000000
#define UTF_8_DOUBLE_BYTE_AND_MASK 0b11100000
#define UTF_8_TRIPLE_BYTE_XOR_MASK 0b11100000
#define UTF_8_TRIPLE_BYTE_AND_MASK 0b11110000
#define UTF_8_QUADRUPLE_BYTE_XOR_MASK 0b11110000
#define UTF_8_QUADRUPLE_BYTE_AND_MASK 0b11111000

int GetUTF_8CharBytes(char c)
{
    if(((c ^ UTF_8_SINGLE_BYTE_XOR_MASK) & UTF_8_SINGLE_BYTE_AND_MASK)
       == UTF_8_SINGLE_BYTE_XOR_MASK)
    {
        return 1;
    }
    else if(((c ^ UTF_8_DOUBLE_BYTE_XOR_MASK) & UTF_8_DOUBLE_BYTE_AND_MASK)
            == UTF_8_DOUBLE_BYTE_XOR_MASK)
    {
        return 2;
    }
    else if(((c ^ UTF_8_TRIPLE_BYTE_XOR_MASK) & UTF_8_TRIPLE_BYTE_AND_MASK)
            == UTF_8_TRIPLE_BYTE_XOR_MASK)
    {
        return 3;
    }
    else if(((c ^ UTF_8_QUADRUPLE_BYTE_XOR_MASK) & UTF_8_QUADRUPLE_BYTE_AND_MASK)
            == UTF_8_QUADRUPLE_BYTE_XOR_MASK)
    {
        return 4;
    }

    return -1;
}

CharPositionVector ParseGenericFile(const char *buffer)
{
    CharPositionVector result = {};

    RainbowStack *s = NULL;

    IntPair cur_pos = { 1, 1 };

    int level = 0;

    for(const char *c = buffer; *c != '\0'; c++)
    {
        int bytes = GetUTF_8CharBytes(*c);
        if(*c == '\n')
        {
            cur_pos.a++;
            cur_pos.b = 1;
        }
        else if(bytes == 1)
        {
            CharPosition p = {};
            p.c = *c;
            p.pair = cur_pos;
            if(*c == '(' || *c == '[' || *c == '{')
            {
                p.level = level;
                s = PushCharPosition(s, p);
                level++;
            }
            else if(p.c == ')' || p.c == ']' || p.c == '}')
            {
                char opening_bracket;
                if(p.c == ')')
                {
                    opening_bracket = '(';
                }
                if(p.c == ']')
                {
                    opening_bracket = '[';
                }
                if(p.c == '}')
                {
                    opening_bracket = '{';
                }
                RainbowStack *copy = s;
                while(copy && copy->data.c != opening_bracket)
                {
                    copy = copy->previous;
                }
                if(copy)
                {
                    while(s != copy)
                    {
                        s = PopCharPosition(s);
                        level--;
                    }
                    CharPosition p2 = s->data;
                    p.level = p2.level;
                    Insert(&result, p);
                    Insert(&result, p2);
                    s = PopCharPosition(s);
                    level--;
                }
            }
            cur_pos.b++;
        }
        else if(bytes == 2)
        {
            ++c;
            cur_pos.b++;
        }
        else if(bytes == 3)
        {
            for(int i = 0; *c != '\0' && i < 2; ++i, ++c);
            cur_pos.b++;
        }
        else if(bytes == 4)
        {
            for(int i = 0; *c != '\0' && i < 3; ++i, ++c);
            cur_pos.b++;
        }
    }

    while(s)
    {
        s = PopCharPosition(s);
    }

    return result;
}

CharPositionVector ParseCFile(const char *buffer)
{
    CharPositionVector result = {};

    RainbowStack *s = NULL;

    IntPair cur_pos = { 1, 1 };

    int level = 0;

    char current_string = '\0';

    const char *multiline_comment = NULL;

    bool line_comment = false;

    for(const char *c = buffer; *c != '\0'; c++)
    {
        int bytes = GetUTF_8CharBytes(*c);
        if(*c == '\n')
        {
            cur_pos.a++;
            cur_pos.b = 1;
            line_comment = false;
        }
        else if(bytes == 1)
        {
            CharPosition p = {};
            p.c = *c;
            p.pair = cur_pos;
            if(line_comment)
            {
            }
            else if(multiline_comment)
            {
                if(*c == '/' && *(c - 1) == '*' && c != multiline_comment + 1)
                {
                    multiline_comment = NULL;
                }
            }
            else if(*c == '*' && c != buffer && *(c - 1) == '/')
            {
                multiline_comment = c;
            }
            else if(*c == '/' && c != buffer && *(c - 1) == '/')
            {
                line_comment = true;
            }
            else if(current_string == '\0')
            {
                if(*c == '(' || *c == '[' || *c == '{')
                {
                    p.level = level;
                    s = PushCharPosition(s, p);
                    level++;
                }
                else if(p.c == ')' || p.c == ']' || p.c == '}')
                {
                    char opening_bracket;
                    if(p.c == ')')
                    {
                        opening_bracket = '(';
                    }
                    else if(p.c == ']')
                    {
                        opening_bracket = '[';
                    }
                    else if(p.c == '}')
                    {
                        opening_bracket = '{';
                    }
                    RainbowStack *copy = s;
                    while(copy && copy->data.c != opening_bracket)
                    {
                        copy = copy->previous;
                    }
                    if(copy)
                    {
                        while(s != copy)
                        {
                            s = PopCharPosition(s);
                            level--;
                        }
                        CharPosition p2 = s->data;
                        p.level = p2.level;
                        Insert(&result, p);
                        Insert(&result, p2);
                        s = PopCharPosition(s);
                        level--;
                    }
                }
                else if(p.c == '\'' || p.c == '\"')
                {
                    current_string = *c;
                }
            }
            else if(current_string)
            {
                if((current_string == '\'' && *c == '\'') &&
                   (*(c - 1) != '\\' || *(c - 2) == '\\'))
                {
                    current_string = '\0';
                }
                else if((current_string == '\"' && *c == '\"') &&
                        (*(c - 1) != '\\' || *(c - 2) == '\\'))
                {
                    current_string = '\0';
                }
            }
            cur_pos.b++;
        }
        else if(bytes == 2)
        {
            ++c;
            cur_pos.b++;
        }
        else if(bytes == 3)
        {
            for(int i = 0; *c != '\0' && i < 2; ++i, ++c);
            cur_pos.b++;
        }
        else if(bytes == 4)
        {
            for(int i = 0; *c != '\0' && i < 3; ++i, ++c);
            cur_pos.b++;
        }
    }

    while(s)
    {
        s = PopCharPosition(s);
    }

    return result;
}

struct MultilineCommentsStack
{
    const char *ptr;
    MultilineCommentsStack *previous;
};

MultilineCommentsStack *PushCommentLevel(MultilineCommentsStack *s, const char *ptr)
{
    MultilineCommentsStack *new_element = (MultilineCommentsStack*)malloc(sizeof(MultilineCommentsStack));

    if(new_element)
    {
        new_element->ptr= ptr;
        new_element->previous = s;
    }

    return new_element;
}

MultilineCommentsStack *PopCommentLevel(MultilineCommentsStack *s)
{
    if(s)
    {
        MultilineCommentsStack *previous = s->previous;

        free(s);

        return previous;
    }

    return NULL;
}

CharPositionVector ParseRustFile(const char *buffer)
{
    CharPositionVector result = {};

    RainbowStack *s = NULL;

    IntPair cur_pos = { 1, 1 };

    int level = 0;

    char current_string = '\0';
    int current_string_count = 0;

    MultilineCommentsStack *multiline_comment = NULL;

    bool line_comment = false;

    for(const char *c = buffer; *c != '\0'; c++)
    {
        int bytes = GetUTF_8CharBytes(*c);
        if(*c == '\n')
        {
            cur_pos.a++;
            cur_pos.b = 1;
            line_comment = false;
        }
        else if(bytes == 1)
        {
            CharPosition p = {};
            p.c = *c;
            p.pair = cur_pos;
            if(line_comment)
            {
            }
            else if(*c == '*' && c != buffer && *(c - 1) == '/')
            {
                multiline_comment = PushCommentLevel(multiline_comment, c);
            }
            else if(multiline_comment)
            {
                if(*c == '/' && *(c - 1) == '*' && c != multiline_comment->ptr + 1)
                {
                    multiline_comment = PopCommentLevel(multiline_comment);
                }
            }
            else if(*c == '/' && c != buffer && *(c - 1) == '/')
            {
                line_comment = true;
            }
            else if(current_string == '\0')
            {
                if(*c == '(' || *c == '[' || *c == '{')
                {
                    p.level = level;
                    s = PushCharPosition(s, p);
                    level++;
                }
                else if(p.c == ')' || p.c == ']' || p.c == '}')
                {
                    char opening_bracket;
                    if(p.c == ')')
                    {
                        opening_bracket = '(';
                    }
                    else if(p.c == ']')
                    {
                        opening_bracket = '[';
                    }
                    else if(p.c == '}')
                    {
                        opening_bracket = '{';
                    }
                    RainbowStack *copy = s;
                    while(copy && copy->data.c != opening_bracket)
                    {
                        copy = copy->previous;
                    }
                    if(copy)
                    {
                        while(s != copy)
                        {
                            s = PopCharPosition(s);
                            level--;
                        }
                        CharPosition p2 = s->data;
                        p.level = p2.level;
                        Insert(&result, p);
                        Insert(&result, p2);
                        s = PopCharPosition(s);
                        level--;
                    }
                }
                else if(p.c == '\'' || p.c == '\"')
                {
                    current_string = *c;
                }
            }
            else if(current_string)
            {
                // NOTE the first check checks for the lifetime specifier
                if((current_string == '\'' && current_string_count == 1) ||
                   ((current_string == '\'' && *c == '\'') &&
                   (*(c - 1) != '\\' || *(c - 2) == '\\')))
                {
                    current_string = '\0';
                    current_string_count = 0;
                }
                else if((current_string == '\"' && *c == '\"') &&
                        (*(c - 1) != '\\' || *(c - 2) == '\\'))
                {
                    current_string = '\0';
                    current_string_count = 0;
                }
                else
                {
                    current_string_count += 1;
                }
            }
            cur_pos.b++;
        }
        else if(bytes == 2)
        {
            ++c;
            cur_pos.b++;
        }
        else if(bytes == 3)
        {
            for(int i = 0; *c != '\0' && i < 2; ++i, ++c);
            cur_pos.b++;
        }
        else if(bytes == 4)
        {
            for(int i = 0; *c != '\0' && i < 3; ++i, ++c);
            cur_pos.b++;
        }
    }


    while(s)
    {
        s = PopCharPosition(s);
    }

    return result;
}

typedef struct {
    char* data;
    unsigned long long length;
} String;

// IMPORTANT to_append MUST be NULL terminated
void AppendCStringToString(String *s, const char* to_append) {
    size_t chars = strlen(to_append);

    s->data = (char *)realloc(s->data, s->length + chars + 1);
    strncpy(s->data + s->length, to_append, chars);
    s->length += chars;
}

#define BUFFER_SIZE 500

int main(int argc, const char **argv)
{
    const char *client = argv[1];
    const char *timestamp = argv[2];
    char mode = argv[3][0];

    IntPair cursor_pair = ParsePair(argv[4]);
    IntPair window_top = ParsePair(argv[5]);
    IntPair window_size = ParsePair(argv[6]);

    const char *filetype = argv[7];

    IntPair window_bottom;
    window_bottom.a = window_top.a + window_size.a;
    window_bottom.b = window_top.b + window_size.b;

    int i = 8;

    const char **colors = argv + i;
    int num_colors = 0;
    for(; i < argc && argv[i][0] != '!'; ++i)
    {
        num_colors++;
    }

    i++;

    const char **background_colors = argv + i;
    int num_background_colors = 0;
    for(; i < argc; ++i)
    {
        num_background_colors++;
    }

    String source_code = {};

    char buffer[BUFFER_SIZE] = {};
    while(int bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE))
    {
        buffer[bytes_read] = '\0';
        AppendCStringToString(&source_code, buffer);
    }

    if(!source_code.data)
    {
        return -1;
    }

    CharPositionVector result;
    if(strcmp(filetype, "c") == 0 || strcmp(filetype, "cpp") == 0)
    {
        result = ParseCFile(source_code.data);
    }
    else if(strcmp(filetype, "rust") == 0)
    {
        result = ParseRustFile(source_code.data);
    }
    else
    {
        result = ParseGenericFile(source_code.data);
    }

    CharPosition cursor_range_a = {};
    CharPosition cursor_range_b = {};

    String output = {};

    sprintf(buffer, "eval -client %s set-option window rainbow %s ", client, timestamp);
    AppendCStringToString(&output, buffer);

    for(int k = result.len - 2; k >= 0; k -= 2)
    {
        CharPosition p = result.array[k + 1];
        CharPosition p2 = result.array[k];
        int level_index = p2.level % (num_colors);
        const char *color = colors[level_index];
        if(IsMaxPair(p.pair, window_top) && IsMinPair(p.pair, window_bottom))
        {
            snprintf(buffer, BUFFER_SIZE, "%d.%d,%d.%d|%s ",
                p.pair.a, p.pair.b, p.pair.a, p.pair.b, color);
            AppendCStringToString(&output, buffer);
        }
        if(IsMaxPair(p2.pair, window_top) && IsMinPair(p2.pair, window_bottom))
        {
            snprintf(buffer, BUFFER_SIZE, "%d.%d,%d.%d|%s ",
                p2.pair.a, p2.pair.b, p2.pair.a, p2.pair.b, color);
            AppendCStringToString(&output, buffer);
        }
        if(mode == '2')
        {
            int level_index = p2.level % (num_background_colors);
            const char *color = background_colors[level_index];
            if(IsRangeVisible(p.pair, p2.pair, window_top, window_bottom))
            {
                snprintf(buffer, BUFFER_SIZE, "%d.%d,%d.%d|default,%s ",
                    p.pair.a, p.pair.b, p2.pair.a, p2.pair.b, color);
                AppendCStringToString(&output, buffer);
            }
        }
        else if(mode == '1')
        {
            if(((cursor_pair.a > p.pair.a) ||
                (cursor_pair.b >= p.pair.b && cursor_pair.a == p.pair.a)) &&
               ((cursor_pair.a < p2.pair.a) ||
                (cursor_pair.b <= p2.pair.b && cursor_pair.a == p2.pair.a)))
            {
                cursor_range_a = p;
                cursor_range_b = p2;
            }
        }
    }

    if(mode == '1')
    {
        if(cursor_range_a.c != 0)
        {
            const char *color = "rgb:003300";
            if(IsRangeVisible(cursor_range_a.pair, cursor_range_b.pair, window_top, window_bottom))
            {
                snprintf(buffer, BUFFER_SIZE, "%d.%d,%d.%d|default,%s ",
                    cursor_range_a.pair.a, cursor_range_a.pair.b,
                    cursor_range_b.pair.a, cursor_range_b.pair.b, color);
                AppendCStringToString(&output, buffer);
            }
        }
    }

    if(output.data)
    {
        printf("%s", output.data);
        free(output.data);
    }

    if(result.array)
    {
        free(result.array);
    }
}
