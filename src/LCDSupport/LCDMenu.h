#pragma once
#include <inttypes.h>

#include <Utility/spVector.h>
#include <LCDSupport/LCDUtility.h>
#include <UserInterface.h>

namespace StarterPack {

namespace LCD {

class menuSystem { 

    //
    // STRUCTURE
    //
    protected:

        struct menuBar;

        //
        // MENU ENTRIES
        //
        struct menuOption {
            const char *option;             // displayed option
            const char *description;        // description

            // action when option is selected
            enum class actionTypeEnum {
                ReturnResult,
                SubMenu,
                ExitToUpperMenu
            };
            actionTypeEnum actionType;

            union {
                uint8_t result;             // if selected, id to return
                menuBar *submenu = nullptr; // if selected, submenu to display
            } actionData;
        };

        //
        // MENU BAR
        //
        class menuBar {
            friend menuSystem;
            private:
                const char *title;             // menu title
                spVector<menuOption> options;  // options to choose from
                uint8_t lastSelectedItem = 0;
                menuBar *parent = nullptr;
            public:
                menuBar( const char *title = nullptr ) {
                    this->title = title;
                }
                ~menuBar() {
                    auto p = options.getFirst();
                    while( p != nullptr ) {
                        Serial.println("delete ");
                        Serial.println(p->option);
                        delete p;
                        p = options.getNext();
                    }
                }
            //
            // ADD
            //
            public:
                void addAction( const char *option, const char *description, uint8_t result ) {
                    // add entry, when selected return result 
                    auto e = new menuOption();
                    e->option = option;
                    e->description = description;
                    e->actionType = menuOption::actionTypeEnum::ReturnResult;
                    e->actionData.result = result;
                    options.insert(e);
                }
                menuBar *addSubmenu( const char *option, const char *description, const char *submenuTitle ) {
                    // add entry, when selected show submenu
                    auto submenu = new menuBar();
                    submenu->title = submenuTitle;
                    submenu->parent = this;

                    auto e = new menuOption();
                    e->option = option;
                    e->description = description;
                    e->actionType = menuOption::actionTypeEnum::SubMenu;
                    e->actionData.submenu = submenu;
                    options.insert(e);

                    return submenu;
                }
                void addExitMenu( const char *option, const char *description ) {
                    // add entry, when selected go back to parent menu
                    auto e = new menuOption();
                    e->option = option;
                    e->description = description;
                    e->actionType = menuOption::actionTypeEnum::ExitToUpperMenu;
                    options.insert(e);
                }
        };

    //
    // INIT
    //
    protected:

        int8_t menuTitleRow, optionsRow, descriptionRow;

    public:

        menuSystem( int8_t menuTitleRow=0, int8_t optionsRow=1, int8_t descriptionRow=2 ) {
            this->menuTitleRow = menuTitleRow;
            this->optionsRow = optionsRow;
            this->descriptionRow = descriptionRow;
        }

        ~menuSystem() {
            deleteItems();
            if ( rootMenu != nullptr ) delete rootMenu;
        }

    //
    //
    //
    protected:

        menuBar *rootMenu = nullptr;
        menuBar *currentMenu = nullptr;

    public:

        menuBar *createRootMenu( const char *submenuTitle ) {
            if (rootMenu != nullptr)
                delete rootMenu;
            rootMenu = new menuBar();
            rootMenu->title = submenuTitle;
            currentMenu = rootMenu;
            return rootMenu;
        }

        void resetMenuSelection() {
            if (rootMenu == nullptr) return;
            resetMenuSelectionCore(rootMenu);
        }

    protected:

        void resetMenuSelectionCore( menuBar *menu ) {
            // set all selection back to 0
            menu->lastSelectedItem = 0;

            auto p = menu->options.getFirst();
            while( p != nullptr ) {
                if (p->actionType==menuOption::actionTypeEnum::SubMenu)
                    resetMenuSelectionCore(p->actionData.submenu);
                p = menu->options.getNext();
            }
        }
    
    //
    // SETTINGS
    //
    public:

        // move focus to 1st non-exit menu option
        // eg. ENTER pressed on [EXIT] option to go back to upper menu
        //     on return visit, useless to still focus on [EXIT]
        //     so move focus to 1st non-exit entry
        // does not apply when user presses [ESCAPE] button
        bool focusTo1stItemOnMenuExit = true;

    //
    // CHOOSER
    //
    protected:

        LCD::optionChooser chooser;

        const char **items = nullptr;
        const char **desc = nullptr;
        uint8_t optionCount = 0;

        void deleteItems() {
            if ( items   != nullptr ) { delete[] items;   items   = nullptr; }
            if ( desc    != nullptr ) { delete[] desc;    desc    = nullptr; }
        }

    //
    // DISPLAY
    //
    protected:

        void prepareMenuForDisplay( menuBar *menu ) {
            // namespace ui = StarterPack::UserInterface;

            deleteItems();

            // count options
            optionCount = 0;
            auto e = menu->options.getFirst();
            while ( e != nullptr ) {
                optionCount++;
                e = menu->options.getNext();
            }

            // allocate memory
            items = new const char*[optionCount];
            desc = new const char*[optionCount];

            // gather options/descriptions
            e = menu->options.getFirst();
            uint8_t i = 0;
            while ( e != nullptr ) {
                items[i] = e->option;
                desc[i] = e->description;
                i++;
                e = menu->options.getNext();
            }

            // this->chooser = new LCD::optionChooser();
            chooser.initN( items, optionCount, optionsRow, menu->lastSelectedItem );
        }

        void displayMenu( menuBar *menu ) {
            namespace ui = StarterPack::UserInterface;

            prepareMenuForDisplay(menu);
            if (!ui::hasScreen()) return;

            if ( menuTitleRow != -1 ) {
                if ( menu->title != nullptr )
                    ui::LCD->printStrAtRow( menuTitleRow, menu->title );
                else
                    ui::LCD->clearRow( menuTitleRow );
            }

            displayOptions(menu);
            // ui::LCD->displayAll();
        }

        void displayOptions( menuBar *menu ) {
            namespace ui = StarterPack::UserInterface;
            if (!ui::hasScreen()) return;
            chooser.display( true );
            if ( descriptionRow != -1 ) {
                if ( desc[chooser.selectedItem] != nullptr )
                    ui::LCD->printStrAtRow( descriptionRow, desc[chooser.selectedItem] );
                else
                    ui::LCD->clearRow( descriptionRow );
            }
            ui::LCD->displayAll();
        }

    //
    // PROMPT
    //
    public:

        static constexpr uint8_t EXIT_VALUE = 0;

        uint8_t prompt() {
            namespace ui = StarterPack::UserInterface;

            if ( currentMenu == nullptr ) return 0;

            // display current menu
            // reprocess, might have added new entries, etc.
            displayMenu( currentMenu );

            ui::waitUntilNothingIsPressed();

            while( true ) {

                uint8_t KEY_PRESSED = ui::getRepeatingKey();

                if ( KEY_PRESSED == ui::kENTER ) {

                    currentMenu->lastSelectedItem = chooser.selectedItem;

                    // find selected option
                    auto selected = currentMenu->options.getFirst();
                    for( int i = 0 ; i < chooser.selectedItem ; i++ ) {
                        selected = currentMenu->options.getNext();
                    }

                    switch( selected->actionType ) {
                    case menuOption::actionTypeEnum::SubMenu:
                        // switch to submenu
                        currentMenu = selected->actionData.submenu;
                        displayMenu( currentMenu );
                        break;
                    case menuOption::actionTypeEnum::ReturnResult:
                        return selected->actionData.result;
                    case menuOption::actionTypeEnum::ExitToUpperMenu:
                        // exit chosen on root menu
                        if ( currentMenu->parent == nullptr )
                            return EXIT_VALUE;
                        if (focusTo1stItemOnMenuExit) {
                            // move focus to 1st non-exit menu option
                            // eg. ENTER pressed on [EXIT] option to go back to upper menu
                            //     on return visit, useless to still focus on [EXIT]
                            //     so move focus to 1st non-exit entry
                            auto index = 0;
                            auto p = currentMenu->options.getFirst();
                            while(p != nullptr) {
                                if (p->actionType!=menuOption::actionTypeEnum::ExitToUpperMenu) {
                                    currentMenu->lastSelectedItem = index;
                                    break;
                                }
                                index++;
                                p = currentMenu->options.getNext();
                            }
                        }
                        // switch to parent
                        currentMenu = currentMenu->parent;
                        displayMenu( currentMenu );
                        break;
                    }

                    // do not accept repeating [ENTER]
                    ui::waitUntilNothingIsPressed();

                } else if ( KEY_PRESSED == ui::kESCAPE ) {

                    // [ESCAPE] pressed on root menu, exit
                    if ( currentMenu->parent == nullptr )
                        return EXIT_VALUE;

                    // switch to parent
                    currentMenu = currentMenu->parent;
                    displayMenu( currentMenu );

                    // do not accept repeating [ESC]
                    ui::waitUntilNothingIsPressed();

                } else if ( KEY_PRESSED == ui::kLEFT ) {
                    
                    if ( chooser.moveLeft() ) {
                        currentMenu->lastSelectedItem = chooser.selectedItem;
                        displayOptions( currentMenu );
                    }

                } else if ( KEY_PRESSED == ui::kRIGHT ) {

                    if ( chooser.moveRight() ) {
                        currentMenu->lastSelectedItem = chooser.selectedItem;
                        displayOptions( currentMenu );
                    }

                }

                if ( backgroundProcess != nullptr ) {
                    if ( !backgroundProcess() )
                        return 0;
                }
            }            
        }

    //
    // EXIT CHECK
    //
    #if defined(ESP8266) || defined(ESP32)
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
    #else
        public:
            typedef bool (*backgroundProcessFunc)();
            backgroundProcessFunc backgroundProcess = nullptr;
            void assignBackgroundProcess( backgroundProcessFunc f ) {
                backgroundProcess = f;
            }
    #endif
        
    };

}

}
