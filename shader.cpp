
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
        float x;
        float y;
    };

    struct Line {
        Point start;
        Point end;
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
    const struct Point top_position =
        { 400, 100 };
    const struct Point bottom_left_position =
        { 300, 400 };
    const struct Point bottom_right_position =
        { 600, 350 };

    // trangle edges
    const struct Line edge1 =
        { top_position, bottom_left_position };
    const struct Line edge2 =
        { bottom_left_position, bottom_right_position };
    const struct Line edge3 =
        { bottom_right_position, top_position };

    const std::vector<Line> edges =
        { edge1, edge2, edge3 };

    // origin
    const struct Point origin =
        { 0, 0 };

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
// ─── KARY TABESH CURVE TEST ─────────────────────────────────────────────────────
//

    bool test_interpolation_using_kary_tabesh_curve ( Point point ) {
        const Line test_line =
            { origin, point };

        int sign = 1;
        for ( auto edge : edges )
            if ( lines_are_intesecting( test_line, edge ) )
                sign = sign * -1;

        return sign == -1;
    }

//
// ─── SCREEN MINIMUM ─────────────────────────────────────────────────────────────
//

    struct Triangle_Information {
        float starting_x;
        float starting_y;
        float ending_x;
        float ending_y;
    };

    Triangle_Information get_triangle_info ( ) {
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

        const Triangle_Information result =
            { minimum_x, minimum_y, maximum_x, maximum_y };

        return result;
    }

//
// ─── OPTIMAL FOR ────────────────────────────────────────────────────────────────
//

    void optimal_triangle_drawing_loop ( ) {
        const auto info =
            get_triangle_info( );

        for ( auto x = info.starting_x; x < info.ending_x; x++ ) {
            for ( auto y = info.starting_y; y < info.ending_y; y++ ) {
                const struct Point point =
                    { x, y };

                if ( test_interpolation_using_kary_tabesh_curve( point ) )
                    glVertex2f( point.x, point.y );
            }
        }
    }

//
// ─── DISPLAY VIEW ───────────────────────────────────────────────────────────────
//

    void display ( ) {
        glColor4f( 1.0f , 0.0f , 0.0f, 0.3f );
        glBegin( GL_POINTS );
            optimal_triangle_drawing_loop( );
        glEnd( );
        glFlush( );
    }

//
// ─── INIT ───────────────────────────────────────────────────────────────────────
//

    void init ( ) {
        glClearColor( 0.0, 0.0, 0.0, 1.0 );
             glClear( GL_COLOR_BUFFER_BIT );
           glColor4f( 1.0, 1.0, 1.0, 1.0 );
             glOrtho( 0.f, screen_width, screen_height, 0.f, 0.f, 1.f );
    }

//
// ─── MAIN ───────────────────────────────────────────────────────────────────────
//

    int main ( int argc, char ** argv ) {
                      glutInit( &argc, argv );
           glutInitDisplayMode( GLUT_SINGLE | GLUT_RGBA );
            glutInitWindowSize( screen_width, screen_height );
        glutInitWindowPosition( 100, 100);
              glutCreateWindow( "Coloring every dot in a triangle using Kary-Tabesh Curve" );
                          init( );
               glutDisplayFunc( display );
                  glutMainLoop( );

        return 0;
    }

// ────────────────────────────────────────────────────────────────────────────────
