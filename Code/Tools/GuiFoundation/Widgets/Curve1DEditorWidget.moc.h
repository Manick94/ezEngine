#pragma once

#include <GuiFoundation/Basics.h>
#include <Foundation/Math/Curve1D.h>
#include <Foundation/Containers/Set.h>
#include <Code/Tools/GuiFoundation/ui_Curve1DEditorWidget.h>

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsItem>

class QMouseEvent;
class QCurve1DEditorWidget;

class ezQCurveControlPoint : public QGraphicsEllipseItem
{
public:
  ezQCurveControlPoint(QGraphicsItem* parent = nullptr);

  virtual int type() const override { return QGraphicsItem::UserType + 1; }

  QCurve1DEditorWidget* m_pOwner;
  ezUInt32 m_uiCurveIdx;
  ezUInt32 m_uiControlPoint;

protected:
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
  virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

  static ezInt32 s_iMovedCps;
};

class ezQCurveSegment : public QGraphicsPathItem
{
public:
  ezQCurveSegment(QGraphicsItem* parent = nullptr);

  virtual int type() const override { return QGraphicsItem::UserType + 2; }

  QCurve1DEditorWidget* m_pOwner;
  ezUInt32 m_uiCurveIdx;
  ezUInt32 m_uiSegment;

  void UpdateSegment();
};

struct ControlPointMove
{
  ezUInt32 curveIdx;
  ezUInt32 cpIdx;
  float x;
  float y;

  bool operator<(const ControlPointMove& rhs) const
  {
    if (curveIdx < rhs.curveIdx)
      return true;
    if (curveIdx > rhs.curveIdx)
      return false;

    return cpIdx < rhs.cpIdx;
  }
};

class EZ_GUIFOUNDATION_DLL QCurve1DEditorWidget : public QWidget, public Ui_Curve1DEditorWidget
{
  Q_OBJECT

public:
  explicit QCurve1DEditorWidget(QWidget* pParent);
  ~QCurve1DEditorWidget();

  void SetNumCurves(ezUInt32 num);

  void SetCurve1D(ezUInt32 idx, const ezCurve1D& curve);
  const ezCurve1D& GetCurve1D(ezUInt32 idx) const { return m_Curves[idx].m_Curve; }

  void FrameCurve();

  //void SetControlPoint(ezUInt32 curveIdx, ezUInt32 cpIdx, float x, float y);
  void SetControlPoints(const ezSet<ControlPointMove>& moves);



signals:
  //void CpAdded(float posX, float value);
  void CpMoved(ezUInt32 curveIdx, ezUInt32 cpIdx, float newPosX, float newPosY);
  void CpDeleted(ezUInt32 curveIdx, ezUInt32 cpIdx);

  //void NormalizeRange();

  void BeginCpChanges();
  void EndCpChanges();

  void BeginOperation();
  void EndOperation(bool commit);


private slots:
  void on_ButtonFrame_clicked();
  void on_SpinPosition_valueChanged(double value);
  void on_SpinValue_valueChanged(double value);
  void on_ButtonNormalize_clicked();
  void onDeleteCPs();

private:
  void UpdateCpUi();

  struct Data
  {
    ezCurve1D m_Curve;
    ezHybridArray<ezQCurveControlPoint*, 10> m_ControlPoints;
    ezHybridArray<ezQCurveSegment*, 10> m_Segments;
  };

  ezHybridArray<Data, 4> m_Curves;
  QGraphicsScene m_Scene;
};
