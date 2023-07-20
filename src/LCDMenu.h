#pragma once
#include <stdint.h>

#include <spVector.h>
#include <LCDUtility.h>
#include <UserInterface.h>

namespace StarterPack {

//
// SET
//

class menuSet { 

        class menuEntry { public:            
            const char *option;
            const char *description;

            uint8_t result;
            menuSet *submenu = nullptr;
            bool backToUpperMenu = false;

            // menuEntry *next = nullptr;
            // inline void assignSubmenu( menuSet &menu ) {
            //     this->submenu = &menu;
        };

        const char *header;
        // menuEntry *options = nullptr;
        spVector<menuEntry> options;
        menuSet *parent = nullptr;

    public:

        ~menuSet() {
            release();
            // delete_NextLinkedList( options );
        }

        menuSet( const char *header = nullptr ) {
            this->header = header;
        }
        
        // menuEntry *
        void add( const char *option, const char *description, uint8_t result ) {
            //return 
            addCore( option, description, result, nullptr, false );
        }

        // menuEntry *
        void add( const char *option, const char *description, menuSet &submenu ) {
            //return
             addCore( option, description, 0, &submenu, false );
        }

        void exitMenu( const char *option, const char *description ) {
            addCore( option, description, 0, nullptr, true );
        }

    //
    // EXIT CHECK
    //
    protected:
        typedef std::function<bool(void)> backgroundProcessDelegate;
        backgroundProcessDelegate backgroundProcess = nullptr;
    public:
        void assignBackgroundProcess( backgroundProcessDelegate backgroundProcess ) {
            this->backgroundProcess = backgroundProcess;
        }        
        void assignBackgroundProcess( bool (*backgroundProcessFunc)() ) {
            this->backgroundProcess = [backgroundProcessFunc]() { return backgroundProcessFunc(); };
        }

    private:

        void addCore( const char *option, const char *description, uint8_t result, menuSet *submenu, bool backToUpperMenu ) {
            menuEntry *e = new menuEntry();
            e->option = option;
            e->description = description;
            e->result = result;
            e->submenu = submenu;
            e->backToUpperMenu = backToUpperMenu;
            if ( submenu != nullptr )
                submenu->parent = this;
            options.insert(e);
            // insertEnd_NextLinkedList( &options, e );
        }

    private:

        const char **items = nullptr;
        const char **desc = nullptr;
        uint8_t optionCount = 0;
        LCDUtility::chooser *chooser = nullptr;

        void release() {
            if ( items   != nullptr ) { delete[] items;   items   = nullptr; }
            if ( desc    != nullptr ) { delete[] desc;    desc    = nullptr; }
            if ( chooser != nullptr ) { delete   chooser; chooser = nullptr; }
        }

        LCDInterface *lcd = nullptr;
        int8_t headerRow, menuRow, descriptionRow;
        menuSet *current = nullptr; // self or submenu

    public:

        void begin( LCDInterface &lcd, int8_t headerRow=1, int8_t menuRow=2, int8_t descriptionRow=3 ) {
            if ( this->lcd != nullptr ) return;
            renew( lcd, headerRow, menuRow, descriptionRow );
        }

        void renew( LCDInterface &lcd, int8_t headerRow=1, int8_t menuRow=2, int8_t descriptionRow=3 ) {

            release();
            current = this;

            this->lcd = &lcd;
            this->headerRow = headerRow;
            this->menuRow = menuRow;
            this->descriptionRow = descriptionRow;

            // count options
            optionCount = 0;
            menuEntry *ptr = options.getFirst();
            // menuEntry *ptr = options;
            while ( ptr != nullptr ) {
                optionCount++;
                ptr = options.getNext();
                // ptr = ptr->next;
            }

            // gather options/descriptions
            items = new const char*[optionCount];
            desc = new const char*[optionCount];

            ptr = options.getFirst();
            // ptr = options;
            uint8_t i = 0;
            while ( ptr != nullptr ) {
                items[i] = ptr->option;
                desc[i] = ptr->description;
                i++;
                ptr = options.getNext();
                // ptr = ptr->next;
            }

            this->chooser = new LCDUtility::chooser( menuRow, optionCount, items );
        }

        uint8_t getSelectedItem() { return chooser->selectedItem; }
        void moveLeft()  { if ( chooser == nullptr ) return; chooser->moveLeft(); }
        void moveRight() { if ( chooser == nullptr ) return; chooser->moveRight(); }

        void display() { // LCDInterface &lcd, int8_t headerRow=1, int8_t menuRow=2, int8_t descriptionRow=3 ) {
            if ( chooser == nullptr ) return;

            if ( headerRow != -1 && header != nullptr )
                lcd->printStrAtRow( headerRow, header );            
            chooser->display( true );
            if ( descriptionRow != -1 )
                lcd->printStrAtRow( descriptionRow, desc[chooser->selectedItem] );
        }

        uint8_t resume() {
            while ( current != nullptr && current != this ) {
                uint8_t r = current->prompt();
                if ( r == 0 ) {
                    // exited, move to parent
                    current = current->parent;
                } else
                    return r;
            }
            current = this;
            return prompt();
        }

        uint8_t prompt() {
            namespace ui = StarterPack::UserInterface;

            if ( chooser == nullptr ) return 0;
            display();

            ui::waitUntilNothingIsPressed();
            uint8_t lastSelectedItem = -1;
            while( true ) {

                uint8_t key = ui::getRepeatingKey();
                
                if ( key == ui::kENTER ) {
                    menuEntry *selected = options.getFirst();
                    // menuEntry *selected = options;
                    for( int i = 0 ; i < chooser->selectedItem ; i++ ) {
                        selected = options.getNext();
                        // selected = selected->next;
                    }
                    if ( selected->submenu != nullptr ) {
                        // open submenu
                        selected->submenu->begin( *lcd, headerRow, menuRow, descriptionRow );
                        uint8_t r = selected->submenu->prompt(); // lcd, headerRow, menuRow, descriptionRow );
                        if ( r != 0 ) {
                            // save where to continue
                            current = selected->submenu;
                            return r;
                        }
                        chooser->display( true );
                        lastSelectedItem = -1;
                    } else if ( selected->backToUpperMenu ) {
                        // as if ESCAPE pressed
                        ui::waitUntilNothingIsPressed();
                        return 0;
                    } else {
                        // return result
                        current = this;
                        return selected->result;
                    }
                    ui::waitUntilNothingIsPressed();
                } else if ( key == ui::kESCAPE ) {
                    // if ( parent != nullptr ) {
                    //     return parent->prompt();
                    // }
                    ui::waitUntilNothingIsPressed();
                    return 0;
                } else if ( key == ui::kLEFT ) {
                    chooser->moveLeft();
                } else if ( key == ui::kRIGHT ) {
                    chooser->moveRight();
                }

                if ( lastSelectedItem != chooser->selectedItem ) {
                    lastSelectedItem = chooser->selectedItem;
                    if ( descriptionRow != -1 )
                        lcd->printStrAtRow( descriptionRow, desc[chooser->selectedItem] );
                    lcd->displayAll();
                }

                if ( backgroundProcess != nullptr ) {
                    if ( !backgroundProcess() )
                        return 0;
                }
            }            
        }
        
    };

}
