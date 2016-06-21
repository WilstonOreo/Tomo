#include <tomo/misc.hpp>
#include "scene/PrintBounds.hpp"
#include <cmath>


namespace tomo
{


/** @brief executes glRotate() for the given angles
 * @tparam COORD coordinate and angle type
 * @param _phi rotation arround the z-axis
 * @param _theta rotation arround the y-axis
 */
template<class COORD> void glRotate(COORD _phi, COORD _theta)
{
  glRotatef(_phi, 0.0, 0.0, 1.0);
  glRotatef(_theta, 0.0, 1.0, 0.0);
}

/** @brief rotate in the given direction
 * @tparam VEC point type including POINT::Scalar as coord type
 * @param _direction vector which points in the desired direction
 */
template<class VEC> void glRotate(const VEC& _direction)
{
  // get coordinate type from vector type
  typedef typename VEC::scalar_type Scalar;
  // pre-calculate vector length
  Scalar length = _direction.length();
  // calculate phi and theta (@link http://de.wikipedia.org/wiki/Kugelkoordinaten#.C3.9Cbliche_Konvention)
  Scalar phi = gex::rad2deg( atan2(_direction.y(), _direction.x()) );
  Scalar theta = (0.0 != length) ? gex::rad2deg(acos( (_direction.z() / length) )) : 0.0;
  // rotate GL world
  glRotate(phi,theta);
}

// realize camera
template<class CAMERA> void realizeCamera(const CAMERA& _camera)
{
  glMatrixMode(GL_MODELVIEW);
  // realize coordinates
  gluLookAt(
    _camera.eye().x(),
    _camera.eye().y(),
    _camera.eye().z(),
    _camera.center().x(),
    _camera.center().y(),
    _camera.center().z(),
    _camera.up().x(),
    _camera.up().y(),
    _camera.up().z()
  );
}
template<class POINT, class COLOR> void drawArrow(
  const std::string& _label,
  const POINT& _p1,
  const POINT& _p2,
  const COLOR& _color,
  bool _drawLabel,
  bool _drawCoords,
  GLfloat _width=1.0,
  GLfloat _arrowR=1.0,
  GLfloat _arrowH=2.0 )
{
  glLineWidth(_width);
/*  glBegin(GL_LINES);
  {
    glColor4fv(_color);
    glVertex3fv(_p1);
    glVertex3fv(_p2);
  }
  glEnd();
  */
  glPushMatrix();
  {
    glTranslatef(COORDS(_p2));
    glRotate(_p2-_p1);
    glColor4fv(_color);
    glTranslatef(0.0,0.0,-_arrowH);
    glutSolidCone(_arrowR, _arrowH, 16.0, 1.0);
    glTranslatef(0.0,0.0,-(_p2-_p1).length()+_arrowH);
    glutSolidCone(_arrowR, (_p2-_p1).length()-_arrowH, 16.0, 1.0);
  }
  glPopMatrix();
  glPushMatrix();
  {
    glTranslatef(COORDS(_p2));
    glRotatef(90.0,1.0,0.0,0.0);
    glScalef(0.01,0.01,0.01);
    if( _drawLabel )
    {
      for(char ch : _label)
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,ch);
    }
    if( _drawCoords )
    {
      glScalef(0.5,0.5,0.5);
      for(char ch : (std::string)_p2)
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,ch);
    }
  }
  glPopMatrix();
  if( _drawCoords )
  {
    glPushMatrix();
    {
      glTranslatef(COORDS(_p1));
      glRotatef(90.0,1.0,0.0,0.0);
      glScalef(0.01,0.01,0.01);
      glScalef(0.5,0.5,0.5);
      for(char ch : (std::string)_p1)
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,ch);
    }
  }
  glPopMatrix();
}

inline void drawAxis(bool _drawLabel, bool _drawCoords)
{
  glMatrixMode(GL_MODELVIEW);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  {
    int length = 10.0;
    // draw axis
    GLfloat axisAlpha=0.7;
    {
      drawArrow("x", GLWidget::Point3(0.0,0.0,0.0), GLWidget::Point3(length,0.0,0.0), GLWidget::Color(1.0,0.0,0.0,axisAlpha), _drawLabel, _drawCoords, 2.0);
      drawArrow("y", GLWidget::Point3(0.0,0.0,0.0), GLWidget::Point3(0.0,length,0.0), GLWidget::Color(0.0,1.0,0.0,axisAlpha), _drawLabel, _drawCoords, 2.0);
      drawArrow("z", GLWidget::Point3(0.0,0.0,0.0), GLWidget::Point3(0.0,0.0,length), GLWidget::Color(0.0,0.0,1.0,axisAlpha), _drawLabel, _drawCoords, 2.0);
    }
  }
  glEnable(GL_DEPTH_TEST);
}

template<class LIGHT> void drawLight( LIGHT _light)
{
  glMatrixMode(GL_MODELVIEW);
  // move light
  glLightfv(GL_LIGHT1, GL_POSITION, _light.eye4());
  //glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, _light.center());
  glEnable(GL_LIGHTING);
}


template<class TRACKER, class COLOR> void drawTracker( const std::string& _name, TRACKER _tracker, const COLOR& _color, bool _drawLabel, bool _drawCoords, typename TRACKER::Scalar _width=1.0)
{
  glMatrixMode(GL_MODELVIEW);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  drawArrow(_name, _tracker.eye(), _tracker.center(), _color, _drawLabel, _drawCoords, _width);
  glEnable(GL_DEPTH_TEST);
}

template<class POINT, class COLOR> void drawSelection( const POINT& _selection, const COLOR& _color )
{
  glMatrixMode(GL_MODELVIEW);
  glDisable(GL_LIGHTING);
  glPointSize(4.0);
  glBegin(GL_POINTS);
  {
    glColor3fv(_color);
    glVertex3fv(_selection);
  }
  glEnd();
}
}
