
//
// Copyright 2018-present, Pouya Kary. All Rights Reserved. <pouya@kary.us>
//

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
// ─── IMPORTS ────────────────────────────────────────────────────────────────────
//

    #ifdef __APPLE__
        #include <GLUT/glut.h>
        #include <stdlib.h>
    #else
        #include <GL/glut.h>
    #endif

    #include <math.h>
    #include <time.h>
    #include <iostream>
    #include <vector>

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

    struct Scann_Line_Info {
        Point L;    // Left  point of intersection in scanning line with triangle
        Point R;    // Right point of intersection in scanning line with triangle
        float LA;   // Length of L to the A
        float LC;   // Length of L to the C
        float RA;   // Length of R to the A
        float RB;   // Length of R to the B
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

    // trangle edges
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
        { 0.0, 1.0, 0.0, 0.8 };
    const struct RGBA B_Lighting =
        { 0.0, 0.0, 1.0, 0.5 };

//
// ─── VECTOR LENGHT ──────────────────────────────────────────────────────────────
//

    float vertex_lenght ( Point a, Point b ) {
        return abs( sqrt( pow( b.x - a.x, 2 ) + pow( b.y - a.y, 2 ) ) );
    }

//
// ─── DO LINES INTERSECT ─────────────────────────────────────────────────────────
//

    bool lines_are_intesecting( Line a, Line b ) {
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
// ─── COMPUTE COLOR ON POSITION ──────────────────────────────────────────────────
//

    RGBA compute_color_on_position ( float x, float y, Scann_Line_Info scann_line ) {
        const struct RGBA color =
            { 1.0, 0.0, 0.0, 1.0 };

        return color;
    }

//
// ─── SET COLOR BASED ON POSITION ────────────────────────────────────────────────
//

    void set_color_based_on_position ( float x, float y, Scann_Line_Info line ) {
        if ( line.L.x <= x && line.R.x >= x ) {
            // const auto color =
            //     compute_color_on_position( x, y, line );

            glColor3f( 1.0, 0.0, 0.0 );
        } else {
            glColor3f( 0.0, 0.0, 0.0 );
        }
    }

//
// ─── GET SCANNING LINE INFORMATION ──────────────────────────────────────────────
//

    Scann_Line_Info get_scann_line_info ( float line_number, Triangle_Info info ) {
        struct Point L =
            { 0.0, line_number };
        struct Point R =
            { screen_width - 1, line_number };
        bool found_L =
            false;

        // getting the L and R
        for ( float x = info.starting_x - 1; x <= info.ending_x; x++ ) {
            const struct Point origin =
                { 0, line_number };
            const struct Point current_position =
                { x, line_number };
            const struct Line checking_line =
                { origin, current_position };

            if ( !found_L && lines_are_intesecting( checking_line, AC ) ) {
                L.x = x;
                found_L = true;
            }

            if ( lines_are_intesecting( checking_line, BA ) ) {
                R.x = x;
                goto done_with_scanning;
            }
        }

        done_with_scanning:

        // Computing the results
        const auto LA =
            vertex_lenght( L, A );
        const auto LC =
            vertex_lenght( L, C );
        const auto RA =
            vertex_lenght( R, A );
        const auto RB =
            vertex_lenght( R, B );

        const struct Scann_Line_Info result =
            { L, R, LA, LC, RA, RB };

        // done
        return result;
    }

//
// ─── OPTIMAL FOR ────────────────────────────────────────────────────────────────
//

    void optimal_triangle_drawing_loop ( ) {
        const auto info =
            get_triangle_info( );

        for ( auto y = info.starting_y + 1; y <= info.ending_y; y++ ) {
            const struct Scann_Line_Info line_info =
                get_scann_line_info( y, info );

            for ( auto x = info.starting_x; x <= info.ending_x; x++ ) {
                set_color_based_on_position( x, y, line_info );
                glVertex2f( x, y );
            }
        }
    }

//
// ─── DISPLAY VIEW ───────────────────────────────────────────────────────────────
//

    void display ( ) {
        // glColor4f( 1.0f , 0.0f , 0.0f, 0.3f );
        glBegin( GL_POINTS );
            optimal_triangle_drawing_loop( );
        glEnd( );
        glFlush( );
    }

//
// ─── INIT ───────────────────────────────────────────────────────────────────────
//

    void init ( ) {
        glClearColor( 0.0, 0.0, 0.0, 0.0 );
             glClear( GL_COLOR_BUFFER_BIT );
           // glColor4f( 1.0, 1.0, 1.0, 1.0 );
             glOrtho( 0.f, screen_width, screen_height, 0.f, 0.f, 1.f );
    }

//
// ─── MAIN ───────────────────────────────────────────────────────────────────────
//

    int main ( int argc, char ** argv ) {
                      glutInit( &argc, argv );
           glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB );
            glutInitWindowSize( screen_width, screen_height );
        glutInitWindowPosition( 100, 100);
              glutCreateWindow( "The Very Basic Gradiant" );
                          init( );
               glutDisplayFunc( display );
                  glutMainLoop( );

        return 0;
    }

// ────────────────────────────────────────────────────────────────────────────────
