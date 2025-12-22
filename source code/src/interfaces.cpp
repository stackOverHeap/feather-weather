#include "screen.h"

////// GLOBALS
/*
const char *ScreenManager::m_settings_keys[] = {
    "Horloge         ",
    "Retour          "
};
*/

static constexpr char * clock_config_keys[] = {
    "Entrer l'annee  ",
    "Entrer le mois  ",
    "Entrer le jour  ",
    "Entrer l'heure  ",
    "Entrer les min. ",
    "Entrer les sec. "
};

/*
Screen_renderer_t *ScreenManager::m_settings_renderer_ptr[2] = {
    &ScreenManager::render_screen_clock_config,
    &ScreenManager::render_screen_temp
};

Input_handler_t *ScreenManager::m_settings_input_handler_ptr[2] = {
    &ScreenManager::handle_input_clock_config,
    &ScreenManager::handle_input_temp
};
*/

////// RENDER FUNCTIONS

void ScreenManager::render_screen_temp (ScreenManager & context)
{
    snprintf(
        context.m_data_ln1, sizeof(context.m_data_ln1), 
        "INT\x4 %02u%c%02u%c EXT\x4",
        context.m_time.hour,
        context.m_time.second % 2 ? ':' : ' ',
        context.m_time.minute,
        context.m_time.hour < 12 ? /*'\x1'*/' ' : /*'\x2'*/' '
    );

    snprintf(
        context.m_data_ln2, sizeof(context.m_data_ln2),
        "%04.1f\xdf""C %c%c %04.1f\xdf""C",
        (double)context.m_temp,
        context.m_signal_strength_warning ? '\x6' : ' ',
        context.m_bat_warning ? '\x5' : ' ',
        (double)context.m_ext_temp
    );
}

void ScreenManager::render_screen_hum (ScreenManager & context)
{
    snprintf(
        context.m_data_ln1, sizeof(context.m_data_ln1),
        "INT\x3 %02u%c%02u%c EXT\x3",
        context.m_time.hour,
        context.m_time.second % 2 ? ':' : ' ',
        context.m_time.minute,
        context.m_time.hour < 12 ? /*'\x1'*/' ' : /*'\x2'*/' '
    );

    snprintf(
        context.m_data_ln2, sizeof(context.m_data_ln2),
        "%04.1f%%  %c%c  %04.1f%%",
        (double)context.m_hum,
        context.m_signal_strength_warning ? '\x6' : ' ',
        context.m_bat_warning ? '\x5' : ' ',
        (double)context.m_ext_hum
    );
}

/*
void ScreenManager::render_screen_settings (ScreenManager & context)
{
    snprintf(
        context.m_data_ln1, sizeof(context.m_data_ln1),
        "%c%s",
        context.m_settings_selector_pos == 0 ? '>' : ' ',
        context.m_settings_keys[0]
    );

    snprintf(
        context.m_data_ln2, sizeof(context.m_data_ln2),
        "%c%s",
        context.m_settings_selector_pos == 1 ? '>' : ' ',
        context.m_settings_keys[1]
    );
}
*/

void ScreenManager::render_screen_clock_config (ScreenManager & context)
{

    strcpy(context.m_data_ln1, clock_config_keys[context.m_clock_config_stage]);

    if (context.m_clock_config_stage == 0) // year configuration
    {
        snprintf(context.m_data_ln2, sizeof(context.m_data_ln2), "%04u            ", (uint16_t)context.m_time.year + 2000);
    }
    else if (context.m_clock_config_stage == 1) // month configuration
    {
        snprintf(context.m_data_ln2, sizeof(context.m_data_ln2), "%02u          ", context.m_time.month);
    }
    else if (context.m_clock_config_stage == 2) // day configuration
    {
        snprintf(context.m_data_ln2, sizeof(context.m_data_ln2), "%02u          ", context.m_time.day);
    }
    else if (context.m_clock_config_stage == 3) // hour configuration
    {
        snprintf(context.m_data_ln2, sizeof(context.m_data_ln2), "%02u          ", context.m_time.hour);
    }
    else if (context.m_clock_config_stage == 4) // minute configuration
    {
        snprintf(context.m_data_ln2, sizeof(context.m_data_ln2), "%02u          ", context.m_time.minute);
    }
    else if (context.m_clock_config_stage == 5) // second configuration
    {
        snprintf(context.m_data_ln2, sizeof(context.m_data_ln2), "%02u          ", context.m_time.second);
    }
}

/*
void ScreenManager::render_screen_calibration_config (ScreenManager & context)
{
    if (context.m_calibration_config_stage == 0 || context.m_calibration_config_stage == 1) // temp offset
    {
        snprintf(context.m_data_ln1, sizeof(context.m_data_ln1), "INT\x4 config EXT\x4");
        snprintf(context.m_data_ln2, sizeof(context.m_data_ln2), "%+03.1f        %+03.1f", context.m_temp_offset, context.m_ext_temp_offset);
    }
    else if (context.m_calibration_config_stage == 2 || context.m_calibration_config_stage == 4) // hum offset
    {
        snprintf(context.m_data_ln1, sizeof(context.m_data_ln1), "INT\x3 config EXT\x3");
        snprintf(context.m_data_ln2, sizeof(context.m_data_ln2), "%+03.1f        %+03.1f", context.m_hum_offset, context.m_ext_hum_offset);
    }
}
// no enough flash ...*/


////// INPUT HANDLERS

void ScreenManager::handle_input_temp (ScreenManager & context, uint8_t input, uint8_t state) 
{
    switch (input)
    {
    case BTN0:
        context.m_renderer = &render_screen_hum;
        context.m_input_handler = &handle_input_hum;
        break;
    
    case BTN1:
        break;
    
    case BTN2:
        break;

    case BTN3:
        context.m_renderer = &render_screen_clock_config;
        context.m_input_handler = &handle_input_clock_config;
        break;
    default:
        break;
    }
}

void ScreenManager::handle_input_hum (ScreenManager & context, uint8_t input, uint8_t state)
{
    switch (input)
    {
    case BTN0:
        context.m_renderer = &render_screen_temp;
        context.m_input_handler = &handle_input_temp;
        break;

    case BTN1:
        break;

    case BTN2:
        break;

    case BTN3:
        context.m_renderer = &render_screen_clock_config;
        context.m_input_handler = &handle_input_clock_config;
        break;

    default:
        break;
    }
}

/*
void ScreenManager::handle_input_settings (ScreenManager & context, uint8_t input, uint8_t state)
{
    switch (input)
    {
    case BTN0: // back
        context.m_renderer = &render_screen_temp;
        context.m_input_handler = &handle_input_temp;
        break;

    case BTN1: // up
        if (context.m_settings_selector_pos > 0)
            context.m_settings_selector_pos--;
        break;

    case BTN2: // down
        if (context.m_settings_selector_pos < 1)
            context.m_settings_selector_pos++;
        break;

    case BTN3: // enter
        context.m_renderer = m_settings_renderer_ptr[context.m_settings_selector_pos];
        context.m_input_handler = m_settings_input_handler_ptr[context.m_settings_selector_pos];
        break;

    default:
        break;
    }
}*/

void ScreenManager::handle_input_clock_config(ScreenManager & context, uint8_t input, uint8_t state)
{
    uint8_t * selection = &context.m_time.year;

    if (!context.m_clock_config_active)
            context.m_clock_config_active = true;

    switch (input)
    {
    case BTN0: // back

        if (context.m_clock_config_stage == 0) {

            context.m_clock_config_active = false;
            context.m_renderer = &render_screen_temp;
            context.m_input_handler = &handle_input_temp;
        } else {

            context.m_clock_config_stage--;
        }
        break;

    case BTN1: // up
        const uint8_t max = context.m_time.upperBound[context.m_clock_config_stage]; 
        if (*selection < max)
            selection[context.m_clock_config_stage]++;
        break;

    case BTN2: // down
        const uint8_t min = context.m_time.lowerBound[context.m_clock_config_stage]; 
        if (*selection > min)
            selection[context.m_clock_config_stage]--;
        break;

    case BTN3: // enter

        if (context.m_clock_config_stage == 5) {

            context.m_clock_config_stage = 0;
            context.m_renderer = &render_screen_temp;
            context.m_input_handler = &handle_input_temp;
            context.m_clock_config_active = false;

            context.m_time_config_cb(
                (uint16_t)(context.m_time.year + 2000), 
                context.m_time.month, 
                context.m_time.day, 
                context.m_time.hour, 
                context.m_time.minute, 
                context.m_time.second
            );
        } else {
            
            context.m_clock_config_stage++;
        }
        break;

    default:
        break;
    }
}

/*
void ScreenManager::handle_input_calibration_config(ScreenManager & context, uint8_t input, uint8_t state)
{
    if (context.m_calibration_config_stage == 0 || context.m_calibration_config_stage == 1)
    {
        switch (input)
        {
        case BTN0: // back
            if (context.m_calibration_config_stage > 1)
                context.m_calibration_config_stage--;
            break; // no possible back when in this menu

        case BTN1: // up
            if (context.m_calibration_config_stage == 0 && context.m_temp_offset < 9.9f)
                context.m_temp_offset += 0.1f;
            if (context.m_calibration_config_stage == 1 && context.m_ext_temp_offset < 9.9f)
                context.m_ext_temp_offset += 0.1f;
            break;

        case BTN2: // down
            if (context.m_calibration_config_stage == 0 && context.m_temp_offset > -9.9f)
                context.m_temp_offset -= 0.1f;
            if (context.m_calibration_config_stage == 1 && context.m_ext_temp_offset > -9.9f)
                context.m_ext_temp_offset -= 0.1f;
            break;

        case BTN3: // enter
            context.m_calibration_config_stage++;
            break;

        default:
            break;
        }
    }
    else if (context.m_calibration_config_stage == 2 || context.m_calibration_config_stage == 3)
    {
        switch (input)
        {
        case BTN0: // back
            context.m_calibration_config_stage--;
            break;

        case BTN1: // up
            if (context.m_calibration_config_stage == 2 && context.m_hum_offset < 9.9f)
                context.m_hum_offset += 0.1f;
            if (context.m_calibration_config_stage == 3 && context.m_ext_hum_offset < 9.9f)
                context.m_ext_hum_offset += 0.1f;
            break;

        case BTN2: // down
            if (context.m_calibration_config_stage == 2 && context.m_hum_offset > -9.9f)
                context.m_hum_offset -= 0.1f;
            if (context.m_calibration_config_stage == 3 && context.m_ext_hum_offset > -9.9f)
                context.m_ext_hum_offset -= 0.1f;
            break;

        case BTN3: // enter
            if (context.m_calibration_config_stage < 3)
                context.m_calibration_config_stage++;
            else
            {
                // Return to settings menu
                context.m_calibration_config_stage = 0;
                context.m_renderer = &render_screen_settings;
                context.m_input_handler = &handle_input_settings;
            }
            break;

        default:
            break;
        }
    }

}

// not enough flash*/