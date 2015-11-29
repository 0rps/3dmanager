#ifndef ISE_EDITITEM_H
#define ISE_EDITITEM_H

#include <QVariant>



/*! �����-��������� �� ������ �������: ��������������, ���������� � �.�.;*/
class  EditItem
{
public:
	EditItem(void);
	~EditItem(void);

	/*! ������ ��������, �� ��������� ModeNothing*/
	enum E_MODE
	{
		ModeNothing,
		ModeView,
		ModeEdit,
		ModeSave,
		ModeCancel,
		ModeAppend,
		ModeRemoval
	};

	void setMode(E_MODE);
    E_MODE mode() const;

	/*! ���������� ���������� ��� ������ �� ������� � E_MODE*/
	virtual bool updateAs( E_MODE ) = 0;

	void setData(const QVariant &);
	const QVariant & dataItem() const;

private:
	E_MODE m_modeNow;
	QVariant m_variant;
};


#endif // ISE_EDITITEM_H
