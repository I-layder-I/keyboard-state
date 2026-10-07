#pragma once
#include "ilayoutprovider.h"

class XkbHelper;

class LayoutProviderFactory {
public:
  static ILayoutProvider *create(XkbHelper *xkb, QObject *parent = nullptr);
};
