#include "widget/Port.h"

#include <tomo/item/detail/PortInterface.hpp>
#include <tomo/item/detail/Source.hpp>
#include <tomo/item/detail/Sink.hpp>
#include <tomo/item/Interface.hpp>
#include <tomo/tool/Connection.hpp>
#include "widget/ElementNode.h"
#include "widget/Connection.h"
#include <boost/algorithm/string/join.hpp>

namespace tomo
{
  namespace widget
  {
    Port::Port(
      ElementNode* _node,
      std::string const& _id,
      item::PortInterface const* _port) :
      ToolchainItem(_node->library(),_node),
      parentNode_(_node),
      disabled_(false),
      highlighted_(false),
      connection_(nullptr),
      id_(_id),
      port_(_port)
    {
    }

    bool Port::drawCaption() const
    {
      if (highlighted()) return true;
      if (disabled()) return false;
      bool _draw = false;
      if (parentNode())
        _draw |= parentNode()->hovered() || (!parentNode()->editor());
      if (connection())
        if (connection()->valid())
          _draw |= connection()->isSelected();

      return _draw;
    }

    QRectF Port::captionRect() const
    {
      if (!parentNode()) return QRectF();

      float r = 40;
      if (direction().x() <= 0)
      {
        return QRectF(radius(),0,radius()*0.5 + r,radius());
      }
      else
      {
        return QRectF(-radius()*1.0 - r,0,radius()*0.5 + r,radius());
      }
      return QRectF();
    }

    QPainterPath Port::shape() const
    {
      QPainterPath path;
      path.addEllipse(rect());
      if (drawCaption())
        path.addRect(captionRect());
      return path;
    }

    bool Port::valid() const
    {
      return true;
    }

    Port::elements_type Port::elements(bool _recursive) const
    {
      return elements_type( {port_->item()});
    }

    float Port::radius()
    {
      return 12.0;
    }

    QRectF Port::rect()
    {
      return QRectF(0,0,radius(),radius());
    }

    QRectF Port::boundingRect() const
    {
      QRectF _rect = rect();
      if (drawCaption())
        _rect |= captionRect();
      return _rect;
    }

    base::ElementId Port::elementId() const
    {
      if (!port()->item()) return base::ElementId("");

      return port()->item()->elementId();
    }

    void Port::adjust()
    {
      if (connection())
        connection()->adjust();
    }

    QPointF Port::direction() const
    {
      if (!parentNode()) return QPointF(0.0,1.0);
      return portType() == SOURCE ? parentNode()->direction() : -parentNode()->direction();
    }

    void Port::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
      if (!painter->isActive()) return;
      auto&& _rect = rect();
      QRadialGradient gradient(QPointF(0,0), _rect.width() / 2);

      QPointF _delta(_rect.width()/1.5,_rect.height()/1.5);
      auto&& _dark = parentNode()->darkColor();
      auto&& _light = parentNode()->lightColor();

      if (disabled())
      {
        _dark.setAlpha(64);
        _light.setAlpha(64);
      }
      else
      {
        _dark = _dark.darker(200);
        _dark.setAlpha(255);
        _light.setAlpha(0);
      }

      gradient.setCenter(_delta);
      gradient.setFocalPoint(_delta);
      gradient.setColorAt(0,_light);
      gradient.setColorAt(1,_dark);

      painter->setBrush(gradient);
      painter->setPen(QPen(_dark,1.0));
      painter->drawEllipse(_rect);

      if (highlighted() || isSelected())
      {
        float r = radius();
        //auto&& rect = QRectF(-r,-r,r*2.0,r*2.0);
        QRadialGradient selectGradient (QPointF(_rect.width()/2.0,_rect.height()/2.0), _rect.width());
        selectGradient.setCenter(_rect.center());
        QColor _color = parentNode()->lightColor();
        _color.setAlpha(0);
        selectGradient.setColorAt(0.9,QColor(255,255,255,0));
        selectGradient.setColorAt(0.5,QColor(255,255,255,128));
        selectGradient.setColorAt(0.3,_color);
        painter->setBrush(selectGradient);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(_rect);
      }

      if (drawCaption())
      {
        auto&& textRect = captionRect();
        auto font = painter->font();
        font.setBold(false);
        font.setPointSize(10.0);
        painter->setBrush(QColor(0,0,0,127));
        painter->setPen(QPen(QColor(0,0,0,127),3.0));
        QPainterPath path;
        path.addText(textRect.bottomLeft(), font, QString().fromStdString(id()));
        if (direction().x() < 0)
        {
          path.translate(textRect.width()-path.boundingRect().width(),0);
        }
        path.translate(0,-path.boundingRect().height()*0.25);

        painter->drawPath(path);
        painter->setBrush(QColor(255,255,255));
        painter->setPen(Qt::NoPen);
        path = QPainterPath();
        path.addText(textRect.bottomLeft(), font, QString().fromStdString(id()));

        if (direction().x() < 0)
          path.translate(textRect.width()-path.boundingRect().width(),0);
        path.translate(0,-path.boundingRect().height()*0.25);
        painter->drawPath(path);
      }
    }

    std::string Port::fullId() const
    {
      if (!parentNode()) return id();
      return 
        parentNode()->prefix() ? 
        parentNode()->id().str() + tool::Connection::idSeparator() + id() :
        id();
    }

    void Port::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
    {
      highlighted_ = true;
      if (parentNode() && !disabled() && highlighted())
      {
        parentNode()->portHovered(this);
        TOMO_DBG(id())
        update();
      }
    }

    void Port::hoverLeaveEvent( QGraphicsSceneHoverEvent * event )
    {
      if (parentNode())
      {
        parentNode()->portHovered(nullptr);
        update();
      }
      highlighted_ = false;
    }

    SourcePort::SourcePort(
      ElementNode* _element,
      std::string const& _id,
      item::SourceInterface const* _source) :
      Port(_element,_id,_source)
    {
      auto&& _supportedTypes = _source->supportedTypes();
      std::vector<std::string> _types;
      for (auto& _type : _supportedTypes)
        _types.push_back(_type.str());
      
      setToolTip(QString("%1 (%2)")
                 .arg(QString().fromStdString(id()))
                 .arg(QString().fromStdString(
                     boost::algorithm::join(_types,", "))));
    
    }

    SinkPort::SinkPort(
      ElementNode* _element,
      std::string const& _id,
      item::SinkInterface const* _sink) :
      Port(_element,_id,_sink)
    {
      auto&& _usedTypes = _sink->usedTypes();
      std::vector<std::string> _types;
      for (auto& _type : _usedTypes)
        _types.push_back(_type.str());
      setToolTip(QString("%1 (%2)")
                 .arg(QString().fromStdString(id()))
                 .arg(QString().fromStdString(
                     boost::algorithm::join(_types,", "))));
    }
  }
}
