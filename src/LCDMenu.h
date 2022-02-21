#pragma once
#include <stdint.h>
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
            menuEntry *next = nullptr;
            menuSet *submenu = nullptr;
            // inline void assignSubmenu( menuSet &menu ) {
            //     this->submenu = &menu;
        };

        const char *header;
        menuEntry *options = nullptr;
        menuSet *parent = nullptr;

    public:

        ~menuSet() {
            release();
        }

        menuSet( const char *header = nullptr ) {
            this->header = header;
        }
        
        // menuEntry *
        void add( const char *option, const char *description, uint8_t result ) {
            //return 
            addCore( option, description, result, nullptr );
        }

        // menuEntry *
        void add( const char *option, const char *description, menuSet &submenu ) {
            //return
             addCore( option, description, 0, &submenu );
        }

    private:

        void addCore( const char *option, const char *description, uint8_t result, menuSet *submenu ) {
            menuEntry *e = new menuEntry();
            e->option = option;
            e->description = description;
            e->result = result;
            e->submenu = submenu;
            if ( submenu != nullptr )
                submenu->parent = this;
            if ( options == nullptr )
                options = e;
            else {
                menuEntry *ptr = options;
                while( ptr->next != nullptr )
                    ptr = ptr->next;
                ptr->next = e;
            }
            // return e;
        }

    private:

        const char **items = nullptr;
        const char **desc = nullptr;
        uint8_t optionCount = 0;
        LCDUtility::chooser *chooser = nullptr;

        void release() {
            if ( items != nullptr ) delete items;
            if ( desc != nullptr ) delete desc;
            if ( chooser != nullptr ) delete chooser;  
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
            menuEntry *ptr = options;
            while ( ptr != nullptr ) {
                optionCount++;
                ptr = ptr->next;
            }

            // gather options/descriptions
            items = new const char*[optionCount];
            desc = new const char*[optionCount];

            ptr = options;
            uint8_t i = 0;
            while ( ptr != nullptr ) {
                items[i] = ptr->option;
                desc[i] = ptr->description;
                i++;
                ptr = ptr->next;
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
                    menuEntry *selected = options;
                    for( int i = 0 ; i < chooser->selectedItem ; i++ )
                        selected = selected->next;
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
            }            
        }
        
    };

}
