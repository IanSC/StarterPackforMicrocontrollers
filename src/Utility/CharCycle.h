#pragma once
#include <stdint.h>
#include <string.h>

#include <Utility/spEnum.h>
#include <Utility/WindowedText.h>

namespace StarterPack {

    class CharCycle {

        using wt = StarterPack::WindowedText;

        //  set allowed characters in [allowedCharacters]
        //  set punctuations using addSymbols()
        //  1 character cycle: cycleUp(), cycleDown()
        //  alphabet jump:     jumpUp(), jumpDown()
        //  reset jump:        resetJump()

        public:

            enum class CharSet : uint8_t {
                None = 0,
                UpperCase = 1,
                LowerCase = 2,
                Numbers = 4,
                Symbols = 8,
                Prefix = 16,
                Space = 32,
                FullAlphabet = UpperCase | LowerCase
            };
            CLASS_ENUM_MANIPULATION(CharSet);

        private:

            // can appear only at the start
            // eg. negative for numbers
            const char *prefixList = nullptr;

            const char *symbolList = nullptr;

        public:

            CharCycle(CharSet allowedCharacters) {
                this->allowedCharacters = allowedCharacters;
            }

            CharSet allowedCharacters = CharSet::None;

            // displayable on LCD screen: !"#$%&'()*+,-./   :;<=>?@   [\\]^_`   {|}
            static constexpr char const *AllSymbols = "!\"#$%&'()*+,-./"  ":;<=>?@"  "[\\]^_`"  "{|}";

            void addPrefixes(const char *prefix) {
                prefixList = prefix;
                if (prefixList == nullptr)
                    allowedCharacters = allowedCharacters & ~CharSet::Prefix;
                else
                    allowedCharacters = allowedCharacters | CharSet::Prefix;
            }

            void addSymbols(const char *symbols) {
                symbolList = symbols;
                if (symbols == nullptr)
                    allowedCharacters = allowedCharacters & ~CharSet::Symbols;
                else
                    allowedCharacters = allowedCharacters | CharSet::Symbols;
            }

            //
            // UPPER/LOWER CASE
            //
            // void changeCase( bool upperCase, char *ch ) {
            //     if ( upperCase )
            //         *ch = toupper(*ch);
            //     else
            //         *ch = tolower(*ch);
            // }

        //
        // CYCLE ONE
        //
        public:

            bool cycleOneUp(char *ch, wt::cursorPositionEnum cursorPosition = wt::cursorPositionEnum::unspecified) {
                return cycleUpCore(ch, cursorPosition, false);
            }

            bool cycleOneDown(char *ch, wt::cursorPositionEnum cursorPosition = wt::cursorPositionEnum::unspecified) {
                return cycleDownCore(ch, cursorPosition, false);
            }

        //
        // CYCLE CORE
        //
        protected:

            bool cycleUpCore(char *ch, wt::cursorPositionEnum cursorPosition, bool jump) {
                if ( allowedCharacters == CharSet::None )
                    return false;
                for(int i=0 ; i<2 ; i++) {
                    if (cursorPosition == wt::cursorPositionEnum::first) {
                        if ( (allowedCharacters & CharSet::Prefix) == CharSet::Prefix ) {
                            if (cycleUpString(ch,prefixList)) return true;
                        }
                    }
                    if ( (allowedCharacters & CharSet::UpperCase) == CharSet::UpperCase ) {
                        if (jump) {
                            if (cycleUpString(ch,alphaJUMP)) return true;
                        } else
                            if (cycleUpRange(ch,'A','Z')) return true;
                    }
                    if ( (allowedCharacters & CharSet::LowerCase) == CharSet::LowerCase ) {
                        if (jump) {
                            if (cycleUpString(ch,alphaJump)) return true;
                        } else
                            if (cycleUpRange(ch,'a','z')) return true;
                    }
                    if ( (allowedCharacters & CharSet::Numbers) == CharSet::Numbers ) {
                        if (cycleUpRange(ch,'0','9')) return true;
                    }
                    if ( (allowedCharacters & CharSet::Symbols) == CharSet::Symbols ) {
                        if (cycleUpString(ch,symbolList)) return true;
                    }
                    if ( (allowedCharacters & CharSet::Space) == CharSet::Space ) {
                        if (cycleUpSpace(ch)) return true;
                    }
                    // full cycle, do another loop
                    *ch = 0;
                }
                return false;
            }

            bool cycleDownCore(char *ch, wt::cursorPositionEnum cursorPosition, bool jump) {
                if ( allowedCharacters == CharSet::None )
                    return false;
                for(int i=0 ; i<2 ; i++) {
                    if ( (allowedCharacters & CharSet::Space) == CharSet::Space ) {
                        if (cycleDownSpace(ch)) return true;
                    }
                    if ( (allowedCharacters & CharSet::Symbols) == CharSet::Symbols ) {
                        if (cycleDownString(ch,symbolList)) return true;
                    }
                    if ( (allowedCharacters & CharSet::Numbers) == CharSet::Numbers ) {
                        if (cycleDownRange(ch,'9','0')) return true;
                    }
                    if ( (allowedCharacters & CharSet::UpperCase) == CharSet::UpperCase ) {
                        if (jump) {
                            if (cycleDownString(ch,alphaJUMP)) return true;
                        } else
                            if (cycleDownRange(ch,'Z','A')) return true;
                    }
                    if ( (allowedCharacters & CharSet::LowerCase) == CharSet::LowerCase ) {
                        if (jump) {
                            if (cycleDownString(ch,alphaJump)) return true;
                        } else
                            if (cycleDownRange(ch,'z','a')) return true;
                    }
                    if (cursorPosition == wt::cursorPositionEnum::first) {
                        if ( (allowedCharacters & CharSet::Prefix) == CharSet::Prefix ) {
                            if (cycleUpString(ch,prefixList)) return true;
                        }
                    }
                    // full cycle, do another loop
                    *ch = 0;
                }
                return false;
            }

        //
        // CYCLE HELPERS
        //
        private:

            static bool cycleUpRange(char *ch, char from, char to) {
                if (*ch==0 || *ch==' ')
                    // ex. 'A';
                    *ch = from;
                else if ( *ch >= from && *ch < to )
                    // ex. 'A' to 'Y'
                    (*ch)++;
                else if ( *ch == to ) {
                    // ex. 'Z'
                    *ch = 0;
                    return false;                
                } else
                    return false;
                return true;
            }

            static bool cycleDownRange(char *ch, char from, char to) {
                if ( *ch==0 || *ch==' ' )
                    // ex. 'Z';
                    *ch = from;
                else if ( *ch <= from && *ch > to )
                    // ex. 'Z' to 'B'
                    (*ch)--;
                else if ( *ch == to ) {
                    // ex. 'A'
                    *ch = 0;
                    return false;
                } else
                    return false;
                return true;
            }

            static bool cycleUpSpace(char *ch) {
                if ( *ch == 0 ) {
                    *ch = ' ';
                    return true;
                }
                return false;
            }

            static bool cycleDownSpace(char *ch) {
                if ( *ch == 0 ) {
                    *ch = ' ';
                    return true;
                }
                return false;
            }

            static bool cycleUpString(char *ch, const char *str) {
                const char *p = str;
                if ( *ch==0 || *ch==' ' )
                    *ch = *p;
                else {
                    while( *p != 0 ) {
                        if ( *p == *ch ) {
                            const char *lastSymbol = str + strlen(str) - 1;
                            if ( p == lastSymbol ) {
                                *ch = 0;
                                return false;
                            } else {
                                p++;
                                *ch = *p;
                                return true;
                            }
                        }
                        p++;
                    }
                    return false;
                }
                return true;
            }

            static bool cycleDownString(char *ch,const char *str) {
                const char *p = str + strlen(str) - 1;
                if ( *ch==0 || *ch==' ' )
                    *ch = *p;
                else {
                    while( p >= str ) {
                        if ( *p == *ch ) {
                            const char *firstSymbol = str;
                            if ( p == firstSymbol ) {
                                *ch = 0;
                                return false;
                            } else {
                                p--;
                                *ch = *p;
                                return true;
                            }
                        }
                        p--;
                    }
                    return false;
                }
                return true;
            }

        //
        // JUMP LOGIC
        //
        private:

            // === OPTION (1) ===
            // while user keep pressing same direction, eg. up up... jump big
            // if switch direction jump: med, sml then single character
            // faster to reach destination, similar to binary tree, but confusing
            //
            //        _abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_
            // big    ||       |        |        |       |        |        |        ||
            // med    |    |       |        |        |       |        |         |     
            //        _abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_
            // sml     | |   |   |   |    |   |    |   |   |   |   |    |     |   | |
            //        ||-x-|-x-|-x-|-x--|-x-|-x--|-x-|-x-|-x-|-x--|-x-|-x--|--x-|-x-|
            //
            // ex. U,U,D,U,U   ==> a,i,e,h,   H
            //     U,U,D       ==> a,i,       E
            //     U,U,U,D,D,U ==> a,i,r,m,k, L
            //     U,U,U,D,D,U ==> a,i,r,m,k, L
            //     U,U,U,D,U   ==> a,i,r,m,   O
            //     U,U,U,U,D,D,D ==> a,i,r,A,v,t,S

            /*
            static constexpr char big0[] = " airAIR09";
            static constexpr char med0[] = " aeimrvAEIMRV059";
            static constexpr char sml0[] = "9 acegkmotvxCEGKMOSVX357";

            void jumpBigOld( bool upDown, char *ch ) {
                if ( *ch == 0 ) *ch = ' ';
                jumpCore( upDown, big0, ch );
            }
            void jumpMedOld( bool upDown, char *ch ) {
                jumpCore( upDown, med0, ch );
            }
            void jumpSmlOld( bool upDown, char *ch ) {
                jumpCore( upDown, sml0, ch );
            }
            bool jumpCore( bool upDown, const char *lookup, char *ch ) {
                // look for character in lookup
                //    up:   get next char, with rollover
                //    down: get prev char, with rollover
                *ch = tolower(*ch);
                char *p = strchr( lookup, *ch );
                if ( p == nullptr ) return false;
                if ( upDown ) {
                    if ( p >= lookup+strlen(lookup)-1 )
                        *ch = lookup[0];
                    else
                        *ch = *(p+1);
                } else {
                    if ( p <= lookup )
                        *ch = *(lookup+strlen(lookup)-1);
                    else
                        *ch = *(p-1);
                }
                return true;
            }
            */

            // === OPTION (2) ===
            // jump several steps if single direction
            // if reversed direction, cycle only 1 character at a time
            //
            //        _abcdefghijklmnopqrstuvwxyz_
            // jumps  ||   |   |  |  |  |  |  |
            //
            // ex. U,U,U,D   ==> a,e,i,   H
            //     U,U       ==> a,       E
            //     U,U,U,U   ==> a,e,i,   L
            //     U,U,U,U   ==> a,e,i,   L
            //     U,U,U,U,U ==> a,e,i,l, O
            //     U,U,U,U,U,U,U,D,D ==>  S

            static constexpr const char* alphaJump = "aeilorux";
            static constexpr const char* alphaJUMP = "AEILORUX";

        //
        // JUMP TRACKING
        //
        private:

            enum class jumpMode {
                idle,
                upBig, downBig,
                small,
                cycle1
            };

            jumpMode jMode = jumpMode::idle;

        public:

            void resetJump() {
                jMode = jumpMode::idle;
            }

            void disableJump() {
                jMode = jumpMode::cycle1;
            }

            void jumpUp(char *ch, wt::cursorPositionEnum cursorPosition = wt::cursorPositionEnum::unspecified) {
                switch( jMode ) {
                case jumpMode::idle:
                case jumpMode::upBig:
                    cycleUpCore( ch, cursorPosition, true );
                    // jumpCharacters( true, ch );
                    jMode = jumpMode::upBig;
                    break;
                case jumpMode::downBig:
                default:
                    cycleUpCore( ch, cursorPosition, false );
                    jMode = jumpMode::cycle1;
                    break;
                }
            }
            
            void jumpDown(char *ch, wt::cursorPositionEnum cursorPosition = wt::cursorPositionEnum::unspecified) {
                switch( jMode ) {
                case jumpMode::idle:
                case jumpMode::downBig:
                    cycleDownCore( ch, cursorPosition, true );
                    // jumpCharacters( false, ch );
                    jMode = jumpMode::downBig;
                    break;
                case jumpMode::upBig:
                default:
                    cycleDownCore( ch, cursorPosition, false );
                    jMode = jumpMode::cycle1;
                }
            }

};

}
