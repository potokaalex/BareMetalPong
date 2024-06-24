#include <stdint.h>
#include "Monitor.h"
#include "MyMath.h"
#include "IOAccess.h"
#include "MyString.h"
#include "MyMemory.h"

#define _MONITOR_HEIGHT 25
#define _MONITOR_WIDTH 80

const uint8_t MONITOR_HEIGHT = _MONITOR_HEIGHT;
const uint8_t MONITOR_WIDTH = _MONITOR_WIDTH;

static uint16_t* _video_memory = (uint16_t*)0xB8000;
static uint16_t _color_attribute = 0;
static uint8_t _cursor_x = 0;
static uint8_t _cursor_y = 0;

static char _monitor_common_buffer[_MONITOR_HEIGHT * _MONITOR_WIDTH];

static uint16_t get_cursor_position()
{
    return _cursor_y * MONITOR_WIDTH + _cursor_x;
}

static void set_char(uint16_t* writeAddr, char c)
{
	*writeAddr = c | _color_attribute;
}

static void set_line_by_char(uint32_t index, char c)
{
	uint16_t* lineAddr = _video_memory + index * MONITOR_WIDTH;

	for (uint32_t i = 0; i < MONITOR_WIDTH; i++)
		set_char(lineAddr + i, c);
}

static void update_cursor_position()
{
    uint16_t position = get_cursor_position();
    io_out_byte(0x3D4, 14);
    io_out_byte(0x3D5, (position >> 8) & 0xFF); // Высший байт позиции
    io_out_byte(0x3D4, 15);
    io_out_byte(0x3D5, position & 0xFF); // Низший байт позиции
}

static void update_scroll()
{
	if(_cursor_y < MONITOR_HEIGHT)
        return;
	
	for (int i = 0; i < MONITOR_WIDTH * (MONITOR_HEIGHT - 1); i++)
		_video_memory[i] = _video_memory[MONITOR_WIDTH + i];

    set_line_by_char(MONITOR_HEIGHT - 1, ' ');
	_cursor_y = MONITOR_HEIGHT - 1;
}

void  monitor_push_char(char c)
{
    uint16_t* writeAddr = _video_memory + get_cursor_position();

	if(c == '\n')
	{
		_cursor_y++;
		_cursor_x = 0;
	}
	else
	{
		set_char(writeAddr, c);
		_cursor_x++;
	}

    if(_cursor_x >= 80)
	{
		_cursor_x = 0;
		_cursor_y++;
	}

	update_scroll();
	update_cursor_position();
}

static void set_colors(uint8_t backColor, uint8_t charColor)
{
	uint8_t attributeuint8_t = (backColor << 4) | (charColor & 0x0F);
	_color_attribute = attributeuint8_t << 8;
}

static void clean()
{
	for (int i = 0; i < MONITOR_HEIGHT; i++)
		set_line_by_char(i, ' ');
}

void monitor_push_string(char* c)
{
    for (uint32_t i = 0; c[i] != '\0'; i++)
        monitor_push_char(c[i]);
}

void monitor_push_uint32(uint32_t value)
{
	string_from_uint32(_monitor_common_buffer, value, 1);
	monitor_push_string(_monitor_common_buffer);
}

void monitor_push_uint32_hex(uint32_t value)
{
	string_from_uint32_hex(_monitor_common_buffer, value, 1);
	monitor_push_string(_monitor_common_buffer);
}

void monitor_push_uint32_bin(uint32_t value)
{
	string_from_uint32_bin(_monitor_common_buffer, value, 1);
	monitor_push_string(_monitor_common_buffer);
}

void monitor_push_int32(int32_t value)
{
	string_from_int32(_monitor_common_buffer, value, 1);
	monitor_push_string(_monitor_common_buffer);
}

void monitor_push_float(float value)
{
	string_from_float(_monitor_common_buffer, value, 1);
	monitor_push_string(_monitor_common_buffer);
}

char* monitor_get_common_buffer()
{
	return _monitor_common_buffer;
}

void monitor_initialize()
{
	set_colors(VGA_COLOR_BLACK, VGA_COLOR_WHITE);
	clean();
	update_cursor_position();
}

void monitor_clean()
{
	clean();
	_cursor_x = 0;
	_cursor_y = 0;
	update_cursor_position();
}

uint32_t monitor_get_cursor_position()
{
	return get_cursor_position();
}

void monitor_set_cursor_position(uint32_t position)
{
	if(position >= MONITOR_WIDTH * MONITOR_HEIGHT)
		return;
	
	_cursor_x = position % MONITOR_WIDTH;
	_cursor_y = position / MONITOR_WIDTH;
	update_cursor_position();
}

void monitor_set_char(char c, uint32_t position)
{
	if(position >= MONITOR_WIDTH * MONITOR_HEIGHT)
		return;

	set_char(_video_memory + position, c);
}