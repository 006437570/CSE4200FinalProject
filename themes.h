#ifndef THEMES_H
#define THEMES_H

struct ThemeColors {
    float hiddenGrid[3];
    float markedGrid[3];
    float bombGrid[3];
    float revealedGrid[3];
    float gridBorder[3];
};

extern ThemeColors themeDefault;
extern ThemeColors theme1;
extern ThemeColors theme2;

#endif