#ifndef APPLICATIONSTYLE_H
#define APPLICATIONSTYLE_H

#include <QString>
#include "Types/TypePreferences.h"

class ApplicationStyle
{
public:
    ApplicationStyle();
    void SetStyle (ePreferenceStyle aStyle) const;
private:
    struct sColorStyle {
        QString _ColorMainWindow,
                _ColorBackground,
                _ColorHover,
                _ColorSelection,
                _ColorText,
                _ColorDisabled;
        sColorStyle() {
            _ColorMainWindow = "black";
            _ColorBackground = "black";
            _ColorHover      = "black";
            _ColorSelection  = "black";
            _ColorText       = "black";
            _ColorDisabled   = "black";
        }
        sColorStyle (const QString& aColorMainWindow,
                     const QString& aColorBackground,
                     const QString& aColorHover,
                     const QString& aColorSelection,
                     const QString& aColorText,
                     const QString& aColorDisabled) {
            _ColorMainWindow = aColorMainWindow;
            _ColorBackground = aColorBackground;
            _ColorHover      = aColorHover;
            _ColorSelection  = aColorSelection;
            _ColorText       = aColorText;
            _ColorDisabled   = aColorDisabled;
        }

    };
    sColorStyle _sDefaultStyle, _sStyleKid;
    QString _FileStyle;
};

#endif // APPLICATIONSTYLE_H
