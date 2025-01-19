#ifndef ATTACKRANGE_H
#define ATTACKRANGE_H

#include <QPainterPath>
#include <QPointF>
#include <QRectF>
#include <QtMath>

class AttackRange { // 攻击实体的范围的抽象类
public:
    virtual ~AttackRange() = default;

    virtual bool contains(const QPointF &center, const QRectF &target) const
        = 0; // 改范围是否包含某对象

    virtual QPainterPath createPath(const QPointF &center) const = 0; // 生成当前范围
    virtual QPainterPath createTrack(const QPointF &center1, const QPointF &center2) const
        = 0; // 生成该范围平移后得到的路径
};

class EllipseRange : public AttackRange { // 椭圆型范围
public:
    explicit EllipseRange(double a, double b);
    ~EllipseRange() = default;

    QPainterPath createPath(const QPointF &center) const override;
    QPainterPath createTrack(const QPointF &center1, const QPointF &center2) const override;

    bool contains(const QPointF &center, const QRectF &target) const override;

protected:
    double a, b; // 半长轴和半短轴
};

class CircleRange : public EllipseRange { // 圆形范围
public:
    explicit CircleRange(double r);
    ~CircleRange() = default;

    QPainterPath createTrack(const QPointF &center1, const QPointF &center2) const override;
};

class SectorRange : public AttackRange { // 扇形范围
public:
    explicit SectorRange(double radius, double startAngle, double spanAngle);

    QPainterPath createPath(const QPointF &center) const override;
    QPainterPath createTrack(const QPointF &center1, const QPointF &center2) const override;

    bool contains(const QPointF &center, const QRectF &target) const override;

protected:
    double radius, startAngle, spanAngle;
};

#endif // ATTACKRANGE_H
