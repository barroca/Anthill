#ifndef CONFMAKER_H
#define CONFMAKER_H

#include <string.h>
#include <stdio.h>

#include "structs.h"

#include "MatLabUtil.h"

#define POLICYLIBFILTERCACHE "label-thresholder-cache.so"
#define POLICYLIBCACHEFILTER "label-thresholder-cache.so"
#define POLICYLIBPOOLTOCACHE "label-antigo.so"
int generateConfXML(char *filterDir, Layout *layout);
#endif
