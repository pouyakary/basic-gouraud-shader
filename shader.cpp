
//
// Copyright 2018-present, Pouya Kary. All Rights Reserved. <pouya@kary.us>
//

//
// ─── IMPORTS ────────────────────────────────────────────────────────────────────
//

    #ifdef __APPLE__
        #include <GLUT/glut.h>
        #include <stdlib.h>
    #else
        #include <GL/glut.h>
    #endif

    #include <math.h>
    #include <vector>
    #include <thread>

// ────────────────────────────────────────────────────────────────────────────────



//
//   ┌──────────────────────────────────────────────────────────────────────────┐
//   │ ****************************  View Buffer  ***************************** │
//   ├──────────────────────────────────────────────────────────────────────────┤
//   │                                                                          │
//   │                                 ┌───┐                                    │
//   │                                 │ A │                                    │
//   │                                 └───┘                                    │
//   │                                  ╱╲                                      │
//   │                                 ╱  ╲                                     │
//   │                                ╱    ╲                                    │
//   │                               ╱      ╲                                   │
//   │                              ╱        ╲                                  │
//   │                             ╱          ╲                                 │
//   │                            ╱            ╲                                │
//   │                           ╱              ╲                               │
//   │                          ╱                ╲                              │
//   │                 ┌───┐   ╱                  ╲    ┌───┐                    │
//   │                 │ L │  ╱                    ╲   │ R │                    │
//   │                 └───┘ ╱                      ╲  └───┘                    │
//   ├──────────────────────╳─────────────*──────────╳──────────────────────────┤
//   │                     ╱          ┌───┐           ╲                         │
//   │                    ╱           │ S │            ╲                        │
//   │                   ╱            └───┘             ╲                       │
//   │                  ╱                                ╲                      │
//   │                 ╱                                  ╲                     │
//   │                ╱                                    ╲                    │
//   │               ╱                                      ╲                   │
//   │              ╱                                        ╲                  │
//   │             ╱                                          ╲                 │
//   │            ╱                                            ╲                │
//   │           ╱─────────┐                                    ╲               │
//   │       ┌───┐         └──────────┐                          ╲              │
//   │       │ C │                    └──────────┐                ╲             │
//   │       └───┘                               └─────────┐       ╲            │
//   │                                                     └────────╲           │
//   │                                                               ┌───┐      │
//   │                                                               │ B │      │
//   │                                                               └───┘      │
//   │                                                                          │
//   └──────────────────────────────────────────────────────────────────────────┘
//


//
// ─── TYPES ──────────────────────────────────────────────────────────────────────
//

    struct Point {
        float x; float y;
    };

    struct Line {
        Point start; Point end;
    };

    struct RGBA {
        float R; float G; float B;  float A;
    };

    struct Triangle_Info {
        float starting_x;   float starting_y;
        float ending_x;     float ending_y;
    };

    struct Scan_Line_Info {
        Point L;    // Left  point of intersection in scanning line with triangle
        Point R;    // Right point of intersection in scanning line with triangle
    };

//
// ─── SETTING CONSTANTS ──────────────────────────────────────────────────────────
//

    // window
    const int screen_width =
        800;
    const int screen_height =
        500;

    // triangle
    const struct Point A =
        { 400, 100 };
    const struct Point C =
        { 200, 400 };
    const struct Point B =
        { 600, 400 };

    // triangle edges
    const struct Line AC =
        { A, C };
    const struct Line CB =
        { C, B };
    const struct Line BA =
        { B, A };
    const std::vector<Line> edges =
        { AC, CB, BA };

    // origin
    const struct Point origin =
        { 0, 0 };

    // Lighting
    const struct RGBA A_Lighting =
        { 1.0, 0.0, 0.0, 1.0 };
    const struct RGBA C_Lighting =
        { 0.0, 1.0, 0.0, 1.0};
    const struct RGBA B_Lighting =
        { 0.0, 0.0, 1.0, 1.0 };

    const struct RGBA BLACK =
        { 0.f, 0.f, 0.f, 0.f };

    const float stronging_ratio =
        4.f;

    // threading
    const auto cores =
        std::thread::hardware_concurrency( );

    RGBA ** screen_matrix;

//
// ─── INIT SCREEN MATRIX ─────────────────────────────────────────────────────────
//

    void init_screen_matrix ( ) {
        screen_matrix =
            new RGBA * [ screen_height ];

        for( int i = 0; i < screen_height; i++ )
            screen_matrix[ i ] = new RGBA[ screen_width ];

        for ( int y = 0; y < screen_height; y++ )
            for ( int x = 0; x < screen_width; x++ )
                screen_matrix[ y ][ x ] = BLACK;
    }

//
// ─── VECTOR LENGTH ──────────────────────────────────────────────────────────────
//

    inline float vertex_length ( Point a, Point b ) {
        return abs( sqrt( pow( b.x - a.x, 2 ) + pow( b.y - a.y, 2 ) ) );
    }

//
// ─── DO LINES INTERSECT ─────────────────────────────────────────────────────────
//

    bool are_lines_intersecting( Line a, Line b ) {
        const auto first_condition =
            ( (   ( b.start.x - a.start.x ) * ( a.end.y - a.start.y )
                - ( b.start.y - a.start.y ) * ( a.end.x - a.start.x ) )
            * (   ( b.end.x   - a.start.x ) * ( a.end.y - a.start.y )
                - ( b.end.y   - a.start.y ) * ( a.end.x - a.start.x )
            ) < 0 );

        const auto second_condition =
            ( (   ( a.start.x - b.start.x ) * ( b.end.y - b.start.y )
                - ( a.start.y - b.start.y ) * ( b.end.x - b.start.x ) )
            * (   ( a.end.x   - b.start.x ) * ( b.end.y - b.start.y )
                - ( a.end.y   - b.start.y ) * ( b.end.x - b.start.x )
            ) < 0 );

        return first_condition && second_condition;
    }

//
// ─── SCREEN MINIMUM ─────────────────────────────────────────────────────────────
//

    Triangle_Info get_triangle_info ( ) {
        float minimum_x = screen_width;
        float minimum_y = screen_height;
        float maximum_x = 0.0;
        float maximum_y = 0.0;

        for ( auto edge : edges ) {
            const auto x =
                edge.start.x;
            const auto y =
                edge.start.y;

            if ( minimum_x > x ) minimum_x = x;
            if ( minimum_y > y ) minimum_y = y;
            if ( maximum_x < x ) maximum_x = x;
            if ( maximum_y < y ) maximum_y = y;
        }

        const Triangle_Info result =
            { minimum_x, minimum_y, maximum_x, maximum_y };

        return result;
    }

//
// ─── COMBINE COLORS ─────────────────────────────────────────────────────────────
//

    RGBA combine_colors ( RGBA color_a, float volume_a, RGBA color_b, float volume_b ) {
        const auto new_R =
            ( ( color_a.R * volume_b ) + ( color_b.R * volume_a ) ) / 2;
        const auto new_G =
            ( ( color_a.G * volume_b ) + ( color_b.G * volume_a ) ) / 2;
        const auto new_B =
            ( ( color_a.B * volume_b ) + ( color_b.B * volume_a ) ) / 2;
        const auto new_A =
            ( ( color_a.A * volume_b ) + ( color_b.A * volume_a ) ) / 2;

        const struct RGBA result =
            { new_R, new_G, new_B, new_A };

        return result;
    }

//
// ─── COMPUTE SCAN LINE LIGHTS ───────────────────────────────────────────────────
//

    RGBA get_RGBA_on_linear_interpolation ( Point position,
                                            Point up,
                                            Point down,
                                            RGBA  up_color,
                                            RGBA  down_color ) {
        const auto up_distance =
            vertex_length( up, position );
        const auto down_distance =
            vertex_length( down, position );
        const auto up_down_size =
            vertex_length( up, down );

        const auto up_volume =
            up_distance / up_down_size;
        const auto down_volume =
            down_distance / up_down_size;

        const auto new_RGBA =
            combine_colors( up_color, up_volume, down_color, down_volume );

        return new_RGBA;
    }

//
// ─── GET IR ─────────────────────────────────────────────────────────────────────
//

    inline RGBA get_IR ( Point position ) {
        return get_RGBA_on_linear_interpolation(
            position, A, B, A_Lighting, B_Lighting
        );
    }

//
// ─── COMPUTE IR ─────────────────────────────────────────────────────────────────
//

    inline RGBA get_IL ( Point position ) {
        return get_RGBA_on_linear_interpolation(
            position, A, C, A_Lighting, C_Lighting
        );
    }

//
// ─── COMPUTE IS ─────────────────────────────────────────────────────────────────
//

    inline RGBA get_IS ( Point position, RGBA IL, RGBA IR, Scan_Line_Info scan_line ) {
        return get_RGBA_on_linear_interpolation(
            position, scan_line.L, scan_line.R, IL, IR
        );
    }

//
// ─── APPLY AMBIENT LIGHT ────────────────────────────────────────────────────────
//

    inline RGBA sharpen_color ( RGBA color ) {
        return {
            color.R * stronging_ratio,
            color.G * stronging_ratio,
            color.B * stronging_ratio,
            color.A
        };
    }

//
// ─── COMPUTE COLOR ON POSITION ──────────────────────────────────────────────────
//

    RGBA compute_color_on_position ( float x, float y, Scan_Line_Info scan_line ) {
        const struct Point position =
            { x, y };
        const auto IL =
            get_IL( position );
        const auto IR =
            get_IR( position );
        const auto IS =
            get_IS( position, IL, IR, scan_line );

        const auto resulting_color =
            sharpen_color( IS );

        return resulting_color;
    }

//
// ─── SET COLOR BASED ON POSITION ────────────────────────────────────────────────
//

    void set_color_based_on_position ( float x, float y, Scan_Line_Info line ) {
        if ( line.L.x <= x && line.R.x >= x && line.L.y > 100 )
            screen_matrix[ (int) y ][ (int) x ] =
                compute_color_on_position( x, y, line );
        else
            screen_matrix[ (int) y ][ (int) x ] = BLACK;
    }

//
// ─── GET SCANNING LINE INFORMATION ──────────────────────────────────────────────
//

    Scan_Line_Info get_scan_line_info ( float line_number, Triangle_Info info ) {
        struct Point L =
            { 0.f, line_number };
        struct Point R =
            { screen_width - 1, line_number };
        bool found_L =
            false;

        // getting the L and R
        for ( float x = info.starting_x - 1; x <= info.ending_x; x++ ) {
            const struct Point origin =
                { 0.f, line_number };
            const struct Point current_position =
                { x, line_number };
            const struct Line checking_line =
                { origin, current_position };

            if ( !found_L && are_lines_intersecting( checking_line, AC ) ) {
                L.x = x;
                found_L = true;
            }

            if ( are_lines_intersecting( checking_line, BA ) ) {
                R.x = x;
                goto done_with_scanning;
            }
        }

        done_with_scanning:

        const struct Scan_Line_Info result =
            { L, R };

        // done
        return result;
    }

//
// ─── DRAWING THREAD TASK ────────────────────────────────────────────────────────
//

    void drawing_thread_task ( float test_start_line,
                               float task_end_line,
                               Triangle_Info info ) {

        for ( auto y = test_start_line; y <= task_end_line; y++ ) {
            const struct Scan_Line_Info line_info =
                get_scan_line_info( y, info );

            for ( auto x = info.starting_x; x <= info.ending_x; x++ ) {
                set_color_based_on_position( x, y, line_info );
            }
        }
    }

//
// ─── OPTIMAL FOR ────────────────────────────────────────────────────────────────
//

    void optimal_triangle_drawing_loop ( ) {
        const auto info =
            get_triangle_info( );
        const auto range =
            info.ending_y - info.starting_y;
        const auto thread_grouping_size =
            range / cores;

        for ( int thread_no = 0; thread_no < cores; thread_no++ ) {
            const auto starting_line =
                info.starting_y + thread_no * thread_grouping_size;
            const auto ending_line =
                info.starting_y + ( thread_no + 1 ) * thread_grouping_size;

            std::thread process (
                drawing_thread_task, starting_line, ending_line, info
            );

            process.join( );
        }
    }

//
// ─── DRAW MATRIX ────────────────────────────────────────────────────────────────
//

    void draw_screen_matrix ( ) {
        glClear( GL_COLOR_BUFFER_BIT );
        glBegin( GL_POINTS );
            for ( int y = 0; y < screen_height; y++ ) {
                for ( int x = 0; x < screen_width; x++ ) {
                    const auto color =
                        screen_matrix[ y ][ x ];

                    glColor3f( color.R, color.G, color.B );
                    glVertex2f( x, y );
                }
            }
        glEnd( );
        glFlush( );
    }

//
// ─── DISPLAY VIEW ───────────────────────────────────────────────────────────────
//

    void display ( ) {
        optimal_triangle_drawing_loop( );
        draw_screen_matrix( );
    }

//
// ─── INIT ───────────────────────────────────────────────────────────────────────
//

    inline void init ( ) {
               glClearColor( 0.f, 0.f, 0.f, 0.f );
                    glClear( GL_COLOR_BUFFER_BIT );
           // glColor4f( 1.0, 1.0, 1.0, 1.0 );
                    glOrtho( 0.f, screen_width, screen_height, 0.f, 0.f, 1.f );

         init_screen_matrix( );
    }

//
// ─── MAIN ───────────────────────────────────────────────────────────────────────
//

    int main ( int argc, char ** argv ) {
                      glutInit( &argc, argv );
           glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB );
            glutInitWindowSize( screen_width, screen_height );
        glutInitWindowPosition( 100, 100);
              glutCreateWindow( "Pouya's Basic Gouraud Interpolating Shader!" );
                          init( );
               glutDisplayFunc( display );
                  glutMainLoop( );

        return 0;
    }

// ────────────────────────────────────────────────────────────────────────────────
