// кря
#pragma once
#include "sysdefs.h"
#include <vector>
#include "view.h"


using namespace std;

#define	W_REGVAL	0x51
#define	W_BITS		0x04
#define	W_EQ		0x04
//#define W_LEDOFF	0x50
#define W_LEDON 	0x50



class DbgColumn;
class DbgCanvas;

class DbgControl
{
	int h_ = 0;
	int y_ = 0;	
	DbgColumn* column_ = nullptr;
	DbgCanvas* canvas_ = nullptr;	

	void check_parent() const;	

protected:
	void draw_reg_frame(const char *title, const u8 *value = nullptr) const;
	void draw_bits_range(int bits) const;
	void draw_bit(char* title, int bits, const char* val_set[], int val) const;
	void draw_bit(char* title, int bits, u8 val) const;
	void draw_hex8_inline(char* title, u8 val) const;
	void draw_dec_inline(char* title, u16 val) const;
	void draw_hex16(char* title, int bits, u16 val) const;
	void draw_hex24(char* title, int bits, u32 val) const;
	void draw_bit_d(char* title, int bits, u8 val) const;
    void draw_bit_h(char * title, int bits, u8 val) const;
	void draw_port(char* title, u8 val) const;
	void draw_hl_port(char prefix, u8 hval, u8 lval, u16 val) const;
	void draw_xhl_port(char prefix, u8 xval, u8 hval, u8 lval) const;
	void draw_led(char* title, u8 on) const;
	void set_xy(int x, int y) const;
	void next_row() const;
	void draw_text_on_line(const char* text, int x, u8 attr) const;

public:
	virtual ~DbgControl() = default;
	explicit DbgControl(int h);

	int get_h() const;
	int get_y() const { return y_; }
	const DbgColumn& get_column() const;

	void set_y(int y);
	void set_parent(DbgColumn *column, DbgCanvas *canvas, int y);
	
	virtual void on_paint() {};
};

class DbgColumn
{
	int y_ = 0;
	const int w_;
	const int x_;

	vector<DbgControl*> controls_ = vector<DbgControl*>();
	DbgCanvas &canvas_;
public:
	DbgColumn(int w, int x, DbgCanvas& canvas);

	int get_w() const;
	int get_x() const;
	int get_y() const;
	
	void reset();
	void move_y(int dy);

	void add_item(DbgControl * control)
	{
		controls_.push_back(control);

		control->set_parent(this, &canvas_, get_y());
		move_y(control->get_h() + 1);
	}
	
	void paint();
};

class DbgCanvas final
{
	DebugView& view_;

	u8 attr_ = 0;
	int base_x_;
	int base_y_;
	int x_ = 0;
	int y_ = 0;
	int cols_[4] = {-1, -1, -1, -1};
	int curr_col_ = 0;
	int last_len_ = 0;

	vector<DbgColumn*> columns_ = vector<DbgColumn*>();
	vector<DbgControl*> controls_ = vector<DbgControl*>();

	void print_dbg(char* line, int color);

public:
	DbgCanvas(DebugView& view, int base_x, int base_y);
	~DbgCanvas();

	DbgCanvas& set_attr(const u8 color);
	DbgCanvas& set_xy(int x, int y);
	DbgCanvas& set_cols(int col0 = -1, int col1 = -1, int col2 = -1, int col3 = -1);
	DbgCanvas& next_row();
	DbgCanvas& next_col();
	DbgCanvas& move_x_to_len();
	DbgCanvas& draw_frame(const int w, int h);
	DbgCanvas& fill_rect(const int w, const int h);
	DbgCanvas& draw_text(const char *msg, int color = -1);
	DbgCanvas& draw_number(const int value, int color = -1);
	DbgCanvas& draw_hex(const unsigned value, int len = 2, int color = -1);

	DbgCanvas& move(int dx, int dy);
	DbgCanvas& set_x(int x);

	void draw_reg_frame(const DbgControl& control, int h, const char* title, const u8 *regval = nullptr);

	DbgColumn& create_column(int w);
	void add_item(DbgControl* control);
	
	void paint();

	
};
