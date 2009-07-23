/*******************************************************************************
 * Ayala Nakashima Barbosa (2004041050)
 * color.h
 *
 * This header file defines macros to control the text colors on a VT100
 * terminal.
 */

#ifndef _COLOR_H
#define _COLOR_H 1

/* some text modes */
#define OFF       "\033[0m"
#define BOLD      "\033[1m"
#define INVISIBLE "\033[8m"

/* text colors */
#define BLACK     "\033[30m"
#define RED       "\033[31m"
#define GREEN     "\033[32m"
#define YELLOW    "\033[33m"
#define BLUE      "\033[34m"
#define PINK      "\033[35m"
#define CIAN      "\033[36m"
#define WHITE     "\033[37m"

/* for output */
#define LINE      OFF BOLD PINK "-------------------\n" OFF  /* divides information */
#define ERR       OFF BOLD RED              /* error message */
#define WAR       OFF BOLD YELLOW           /* warning message */
#define OUT       OFF BOLD                  /* ouput message */

#endif /* _COLOR_H */
