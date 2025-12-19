#include <cairo.h> // (with # and <>)
#include <cairo-pdf.h>
#include <string>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

const double side = 20;

const double center_x_i = 72*8.5/2-180;
const double center_y_i = 72*11/2+230;
const double center_x_c = 72 * 8.5/2;
const double center_y_c = 72*11/2;

double center_x = 0;
double center_y = 0;


int parity(std::string shape){
  int p = 0;
  for (char c: shape){
    if (c == '+') p++;
    if (c == '-') p--;
  }
  return p;
}

void draw_hex(cairo_t *cr, std::string shape = "00-000", int ix=0, int iy=0){
    double edge = sqrt(3) / 2.0;
    double c_x = center_x + ix*side*2*edge + iy*side*edge;
    double c_y = center_y - iy*side*1.5;

    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    int angle_deg = -90;
    double rad = angle_deg * (M_PI / 180.0);
    cairo_move_to(cr, c_x + side  * std::cos(rad), c_y + side * std::sin(rad));
    double cur_x = c_x + side  * std::cos(rad);
    double cur_y = c_y + side * std::sin(rad);
    for (int i = 1; i <= 6; ++i){
      if (shape[i-1] == '-' || shape[i-1] == 'a'|| shape[i-1] == 'b'|| shape[i-1] == 'c'){
        double x = cur_x + side/3 * std::cos( (60*i-30)*M_PI / 180.0);
        double y = cur_y + side/3 * std::sin( (60*i-30)*M_PI / 180.0);
        cairo_line_to(cr, x, y);
        
        double x_mult = 0.6;
        if (shape[i-1] == 'a'){
          x_mult = 0.8;
        } else if (shape[i-1] == 'c'){
          x_mult = 0.4;
        }

        x = c_x + side * std::cos( (60*i-120)* M_PI / 180.0) * x_mult*edge;
        y = c_y + side * std::sin( (60*i-120)* M_PI / 180.0) * x_mult*edge;
        cairo_line_to(cr, x, y);

        x = cur_x + 2*side/3 * std::cos( (60*i-30)*M_PI / 180.0);
        y = cur_y + 2*side/3 * std::sin( (60*i-30)*M_PI / 180.0);
        cairo_line_to(cr, x, y);
//        std::cout << x << y << "\n";

      } else if (shape[i-1] == '+' || shape[i-1] == 'A' || shape[i-1] == 'B' || shape[i-1] == 'C') {
        double x = cur_x + side/3 * std::cos( (60*i-30)*M_PI / 180.0);
        double y = cur_y + side/3 * std::sin( (60*i-30)*M_PI / 180.0);
        cairo_line_to(cr, x, y);
        
        double x_mul2 = 1.4;
        if (shape[i-1] == 'A'){
          x_mul2 = 1.2;
        } else if (shape[i-1] == 'C'){
          x_mul2 = 1.6;
        }

        x = c_x + side * std::cos( (60*i-120)* M_PI / 180.0) * x_mul2*edge;
        y = c_y + side * std::sin( (60*i-120)* M_PI / 180.0) * x_mul2*edge;
        cairo_line_to(cr, x, y);

        x = cur_x + 2*side/3 * std::cos( (60*i-30)*M_PI / 180.0);
        y = cur_y + 2*side/3 * std::sin( (60*i-30)*M_PI / 180.0);
        cairo_line_to(cr, x, y);

      }
      angle_deg = 60*i-90;
      double angle_rad = angle_deg * (M_PI / 180.0);
      cairo_line_to(cr, c_x + side  * std::cos(angle_rad), c_y + side * std::sin(angle_rad));
      cur_x = c_x + side  * std::cos(angle_rad);
      cur_y = c_y + side * std::sin(angle_rad);

    }
}

std::string transform(std::string shape, int rot=0, bool ref=false){
  if (ref){
    std::reverse(shape.begin(), shape.end());
  }
  for (int i = 0; i < rot; ++i){
    char fs = shape[0];
    shape.erase(0,1);
    shape += fs;
  }
  return shape;
}

int iso_to(std::vector<std::string> v, std::string shape){
  for (unsigned int i = 0; i < v.size(); ++i){
    bool ref = true;
    for (int j = 0; j < 2; ++j){
      for (int k = 0; k < 6; ++k){
        if (transform(v[i], k, ref) == shape){
          return i;
        }
      }
      ref = true;
    }
  }
  return -1;
}


void draw_hex_fill(cairo_t *cr, std::string shape = "00-000", int ix=0, int iy=0, std::vector<std::string> v = {}){
    draw_hex(cr, shape, ix, iy);
    if (iso_to(v, shape) == 0){
      cairo_set_source_rgb (cr, 0.7, 0.5, 1);
      //cairo_set_source_rgb (cr, 1, 0, 0);
    } else if (iso_to(v, shape) == 1){
      cairo_set_source_rgb (cr, 1, 0.3, 0.8);
      //cairo_set_source_rgb (cr, 0.3, 0.3, 1);
    } else if (iso_to (v, shape) == 2){
      cairo_set_source_rgb (cr, 1, 0.8, 0.95);
      //cairo_set_source_rgb (cr, 1, 1, 0);
    } else {
      cairo_set_source_rgb (cr, 0.5, 0, 0.3);
      //cairo_set_source_rgb (cr, 0, 0.8, 0);
    }
    cairo_fill( cr );
}

void draw_hex_outline(cairo_t *cr, std::string shape = "00-000", int ix=0, int iy=0){
    draw_hex(cr, shape, ix, iy);
    cairo_set_line_width (cr, 1);
    cairo_stroke (cr);
}


int main() {
  cairo_surface_t *pdf;
  pdf = cairo_pdf_surface_create( "out.pdf", 8.5*72, 11*72 );
	cairo_t *cr = cairo_create( pdf );

  int num_pages, num_shapes, n_shapes_in_drawing;
  std::string s;
  std::string s3;
  std::cin >> num_pages >> s;
  
  // corona or tt mode
  if (s == "coronas") {
    center_x = center_x_c;
    center_y = center_y_c;
  } else if (s == "tt") {
    center_x = center_x_i;
    center_y = center_y_i;
  }

  for (int i = 0; i < num_pages; ++i){

    std::cin >> num_shapes;
    s3 = "";
    std::vector<std::string> v;

    for (int j = 0; j < num_shapes; ++j){
      std::cin >> s;
      v.push_back(s);
      s3 += s;
      if (j != num_shapes-1){
        s3 += ",";
      }
    }
    std::cin >> n_shapes_in_drawing;

    cairo_save ( cr );
    cairo_select_font_face (cr, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size (cr, 32.0);
    cairo_set_source_rgb (cr, 0.0, 0.0, 1.0);
    cairo_move_to (cr, 10.0, 50.0);
    cairo_show_text (cr, s3.c_str());
    
    for (int j = 0; j < n_shapes_in_drawing; ++j){
      int x, y;
      std::string shape;
      std::cin >> x >> y >> shape;
      draw_hex_fill(cr, shape, x, y, v);
      draw_hex_outline(cr, shape, x, y);
    }
    cairo_restore( cr );
    cairo_surface_show_page ( pdf );
  }

  cairo_destroy (cr);

  cairo_surface_finish( pdf );
  cairo_surface_destroy( pdf );


  return 0;
}

