//
// Created by Radek on 30.12.2024.
//

#include "PPU.h"

#include <cassert>
#include <string.h>

namespace ppu {
    constexpr u16 PPUCTRLAddress = 0x2000;
    constexpr u16 PPUMASKAddress = 0x2001;
    constexpr u16 PPUSTATUSAddress = 0x2002;
    constexpr u16 OAMADDRAddress = 0x2003;
    constexpr u16 OAMDATAAddress = 0x2004;
    constexpr u16 PPUSCROLLAddress = 0x2005;
    constexpr u16 PPUADDRAddress = 0x2006;
    constexpr u16 PPUDATAAddress = 0x2007;
    constexpr u16 OAMDMAAddress = 0x2008;
}

using namespace ppu;

PPU::PPU(Memory* shared) {
    this->sharedMemory = shared;
    init(sharedMemory);

    palette = {{
        {0x80, 0x80, 0x80}, {0x00, 0x3D, 0xA6}, {0x00, 0x12, 0xB0}, {0x44, 0x00, 0x96},
        {0xA1, 0x00, 0x5E}, {0xC7, 0x00, 0x28}, {0xBA, 0x06, 0x00}, {0x8C, 0x17, 0x00},
        {0x5C, 0x2F, 0x00}, {0x10, 0x45, 0x00}, {0x05, 0x4A, 0x00}, {0x00, 0x47, 0x2E},
        {0x00, 0x41, 0x66}, {0x00, 0x00, 0x00}, {0x05, 0x05, 0x05}, {0x05, 0x05, 0x05},
        {0xC7, 0xC7, 0xC7}, {0x00, 0x77, 0xFF}, {0x21, 0x55, 0xFF}, {0x82, 0x37, 0xFA},
        {0xEB, 0x2F, 0xB5}, {0xFF, 0x29, 0x50}, {0xFF, 0x22, 0x00}, {0xD6, 0x32, 0x00},
        {0xC4, 0x62, 0x00}, {0x35, 0x80, 0x00}, {0x05, 0x8F, 0x00}, {0x00, 0x8A, 0x55},
        {0x00, 0x99, 0xCC}, {0x21, 0x21, 0x21}, {0x09, 0x09, 0x09}, {0x09, 0x09, 0x09},
        {0xFF, 0xFF, 0xFF}, {0x0F, 0xD7, 0xFF}, {0x69, 0xA2, 0xFF}, {0xD4, 0x80, 0xFF},
        {0xFF, 0x45, 0xF3}, {0xFF, 0x61, 0x8B}, {0xFF, 0x88, 0x33}, {0xFF, 0x9C, 0x12},
        {0xFA, 0xBC, 0x20}, {0x9F, 0xE3, 0x0E}, {0x2B, 0xF0, 0x35}, {0x0C, 0xF0, 0xA4},
        {0x05, 0xFB, 0xFF}, {0x5E, 0x5E, 0x5E}, {0x0D, 0x0D, 0x0D}, {0x0D, 0x0D, 0x0D},
        {0xFF, 0xFF, 0xFF}, {0xA6, 0xFC, 0xFF}, {0xB3, 0xEC, 0xFF}, {0xDA, 0xAB, 0xEB},
        {0xFF, 0xA8, 0xF9}, {0xFF, 0xAB, 0xB3}, {0xFF, 0xD2, 0xB0}, {0xFF, 0xEF, 0xA6},
        {0xFF, 0xF7, 0x9C}, {0xD7, 0xE8, 0x95}, {0xA6, 0xED, 0xAF}, {0xA2, 0xF2, 0xDA},
        {0x99, 0xFF, 0xFC}, {0xDD, 0xDD, 0xDD}, {0x11, 0x11, 0x11}, {0x11, 0x11, 0x11}
    }};
}

void PPU::init(Memory* shared) {
    frameReady = false;
    scanline = 0;

    regs.T = 0;
    regs.V = 0;
    regs.W = 0;
    regs.X = 0;

    regs.OAMAddr = &shared->getReference(OAMADDRAddress);
    regs.OAMData = &shared->getReference(OAMDATAAddress);
    regs.PPUAddr = &shared->getReference(PPUADDRAddress);
    regs.PPUControl = &shared->getReference(PPUCTRLAddress);
    regs.PPUData = &shared->getReference(PPUDATAAddress);
    regs.PPUMask = &shared->getReference(PPUMASKAddress);
    regs.PPUScroll = &shared->getReference(PPUSCROLLAddress);
    regs.PPUStatus = &shared->getReference(PPUSTATUSAddress);

    cycle = 0;

    entireFrameBuffer.resize(resolution.x * resolution.y);
    frameBuffer1.resize(resolution.x * resolution.y);
    frameBuffer2.resize(resolution.x * resolution.y);
    frameBufferBG.resize(resolution.x * resolution.y);

    spriteBuffer.resize(8);

    oam.resize(oamSize);
}

void PPU::update() {
    // // Symulacja renderowania wszystkich widocznych linii
    // for (int y = 0; y < resolution.y; ++y) {
    //     for (int x = 0; x < resolution.x; ++x) {
    //         renderPixel(x, y);
    //     }
    // }

    for (int scanline = 0; scanline < resolution.y; ++scanline) {
        renderBackgroundLine(scanline);
    }

    if ((*regs.PPUAddr & 0x1f) == 0x1f)
    {
        // Wrap to the next name table
        *regs.PPUAddr &= ~0x1f;
        *regs.PPUAddr ^= 0x0400;
    }
    else
    {
        *regs.PPUAddr++;
    }

    // Po zakończeniu ramki oznacz ją jako gotową
    frameReady = true;
}

std::vector<u8> PPU::getFrameBuffer() const {
    if(entireFrameBuffer == frameBuffer1)
        return frameBuffer2;

    return frameBuffer1;
}

std::vector<uint32_t> PPU::getFrame() const {
    std::vector<u32> frame;
    std::vector<u8> frameBuffer = getFrameBuffer();

    frame.resize(frameBuffer.size());
    for (int i = 0; i < frameBuffer.size(); ++i) {
        frame[i] = palette[frameBuffer[i] & 0xff].makeu32();
    }

    return frame;
}

void PPU::renderPixel(u16 x, u16 y) {
    if (x < 0 || x >= resolution.x || y < 0 || y >= resolution.y) {
        return; // Ignoruj piksele poza ekranem
    }

    // Oblicz pozycję kafelka (8x8)
    int tileX = x / 8;
    int tileY = y / 8;

    // Offset pikseli w kafelku
    int pixelX = x % 8;
    int pixelY = y % 8;

    // Oblicz adres kafelka w pamięci
    u16 tileIndex = (tileY * 32 + tileX) & 0x3FF; // Tabela kafelków mieści się w 10 bitach
    u16 tileAddress = PPUCTRLAddress + tileIndex;

    // Pobierz dane kafelka
    u8 tileLow = sharedMemory->getReference(tileAddress + pixelY);
    u8 tileHigh = sharedMemory->getReference(tileAddress + pixelY + 8);

    // Oblicz kolor piksela
    u8 paletteIndex = ((tileHigh >> (7 - pixelX)) & 1) << 1 | ((tileLow >> (7 - pixelX)) & 1);
    Color c = palette[paletteIndex];

    // Zapisz kolor do bufora klatki
    //entireFrameBuffer[y * resolution.x + x] = c;
}

void PPU::renderBackgroundLine(int scanline) {
    if (scanline < 0 || scanline >= resolution.y) {
        return; // Ignoruj linie poza widocznym ekranem
    }

    for (int x = 0; x < resolution.x; ++x) {
        // Oblicz pozycję w Name Table
        int tileX = (x + regs.X) / 8;
        int tileY = (scanline + regs.yScroll) / 8;

        u16 nameTableBase = 0x2000 + ((*regs.PPUControl & 0b11) * 0x400); // Wybranie właściwej Name Table
        u16 tileIndex = (tileY % 30) * 32 + (tileX % 32); // Indeks kafelka w Name Table
        u8 tileID = sharedMemory->getReference(nameTableBase + tileIndex);

        // Oblicz adres w Pattern Table
        u16 patternTableBase = (*regs.PPUControl & 0b0001) ? 0x1000 : 0x0000; // Wybranie Pattern Table
        u16 tileAddress = patternTableBase + tileID * 16;

        // Wczytaj dane kafelka
        int pixelY = (scanline + regs.yScroll) % 8; // Wiersz w kafelku
        u8 tileLow = sharedMemory->getReference(tileAddress + pixelY);
        u8 tileHigh = sharedMemory->getReference(tileAddress + pixelY + 8);

        // Oblicz kolor piksela
        int pixelX = (x + regs.X) % 8; // Kolumna w kafelku
        u8 paletteIndex = ((tileHigh >> (7 - pixelX)) & 1) << 1 | ((tileLow >> (7 - pixelX)) & 1);

        // Pobierz informację o atrybucie (palecie)
        int attributeX = tileX / 4;
        int attributeY = tileY / 4;
        u16 attributeAddress = nameTableBase + 0x03C0 + (attributeY * 8 + attributeX);
        u8 attributeByte = sharedMemory->getReference(attributeAddress);

        // Przesunięcie w bajcie atrybutów
        int shift = ((tileY % 4 >= 2) ? 4 : 0) + ((tileX % 4 >= 2) ? 2 : 0);
        u8 paletteSelect = (attributeByte >> shift) & 0b11;

        // Pobierz kolor z palety
        u8 colorIndex = paletteSelect * 4 + paletteIndex;
        Color c = palette[colorIndex]; // Adres palety

        // Zapisz kolor do bufora
        //entireFrameBuffer[scanline * resolution.x + x] = c;
    }
}

void PPU::render() {

    for (auto & x : entireFrameBuffer) {
        x = 0;
    }

    if(scanline <= 239) {
        tilesPipeline();
        spritesPipeline();
    }
    else if(scanline == 240) {

    }
    else if(scanline == 241) {

    }
    else if(scanline < 261) {
        if(scanline == 241 && cycle == 1) {

        }

        if (scanline == 260 && cycle > 329) {

        }
    }
    else {
        if(scanline == 261) {
            if(cycle == 0) {

            }
            else if(cycle == 1) {

            }
        }

        if(cycle == 340) {
            
        }
    }
}

void PPU::tilesPipeline() {
    // No need to fetch anything if rendering is off
    if (!regs.showBackground()) return;

    if (cycle == 0)
    {
    }
    else if (cycle < 257)
    {
        fetchTile();

        if (cycle == 256)
        {
            if ((*regs.PPUAddr & 0x7000) != 0x7000)
            {
                // Increase fine Y position (within tile)
                *regs.PPUAddr += 0x1000;
            }
            else
            {
                *regs.PPUAddr &= ~0x7000;

                // == row 29?
                if ((*regs.PPUAddr & 0x3e0) != 0x3a0)
                {
                    // Increase coarse Y position (next tile)
                    *regs.PPUAddr += 0x20;
                }
                else
                {
                    // wrap around
                    *regs.PPUAddr &= ~0x3e0;

                    // switch to another vertical name table
                    *regs.PPUAddr ^= 0x0800;
                }
            }
        }
    }
    else if (cycle < 321)
    {
        if (cycle == 257)
        {
            // Reset horizontal position
            // This includes resetting horizontal name table (2000~2400, 2800~2c00)
            // NNYY YYYX XXXX
            //  ^      ^ ^^^^
            *regs.PPUAddr = (*regs.PPUAddr & 0xfbe0) | (regs.T & ~0xfbe0);
            xOffset = 0;
        }

        // fetch tile data for sprites on the next scanline
    }
    else if (cycle < 337)
    {
        // first two tiles for next scanline
        fetchTile();
    }
    else // 337->340
    {
        // fetch two bytes - no need to emulate for now
    }
}

void PPU::spritesPipeline() {
    if (!regs.showSprites())
        return;

    // sprite never show up in scanline 0
    if (scanline == 0)
        return;

    // @TODO - Sprite 0 hit testing
    if (cycle == 0)
    {
        lastSpriteID = 0;
        hasSprite0 = false;
        memset(spriteBuffer.data(), 0xff, spriteBuffer.size() * sizeof(Sprite));
        //_sprite_overflow = false;
        regs.setSpriteOverflow(false);
    }
    else if (cycle < 65)
    {
        // initialize secondary OAM (_sprite_buf)
        // we've already done this earlier and the side effect isn't observable anyway

        // NOTE: We can set this conditionaly but it's simply faster always to set it
        regs.maskOAMRead = true;
    }
    else if (cycle < 257)
    {
        // fetch tile data for sprites on the next scanline

        // NOTE: We can set this conditionaly but it's simply faster always to set it
        regs.maskOAMRead = false;

        u8 sprite_cycle = (uint8_t) (cycle - 65);
        u8 sprite_id = sprite_cycle / 2;

        // Cycle 65-256 has more than enough cycles to read all 64 sprites - but it appears that this
        // pipeline would synchronize with the background rendering pipeline so that sprites rendering
        // happens later and can freely overwrite background if needed.
        // Skip if we are already past 64
        if (sprite_id >= 64)
            return;

        if ((cycle % 2) == 0)
        {
            // even cycle - write to secondary OAM
            // if in range
            u8 spriteHeight = regs.use8x16Sprites() ? 16 : 8;
            if (lastSpriteY + 1 <= scanline && scanline < lastSpriteY + 1 + spriteHeight)
            {
                if (sprite_id == 0)
                    hasSprite0 = true;

                if (lastSpriteID >= 8)
                    regs.setSpriteOverflow(true);
                else
                    spriteBuffer[lastSpriteID++] = *getSprite(sprite_id);
            }
        }
        else
        {
            // odd cycle - read from primary OAM
            lastSpriteY = getSprite(sprite_id)->y;
        }
    }
    else if (cycle < 321)
    {
        uint8_t sprite_cycle = (uint8_t) (cycle - 257);
        uint8_t sprite_id = sprite_cycle / 8;
        if (sprite_cycle % 8 == 4)
        {
            if (sprite_id < lastSpriteID)
                fetch_sprite(sprite_id);
        }
    }
    else // 337->340
    {
     // fetch two bytes - no need to emulate for now
    }
}

void PPU::fetchTile() {
    u16 scanline_render_cycle = 0;
    u16 cur_scanline = scanline;

    if (scanline > 320)
    {
        // this is prefetch cycle 321~336 for next scanline
        scanline_render_cycle = scanline - 321;
        cur_scanline = (cur_scanline + 1) % resolution.y;
    }
    else
    {
        // account for the prefetch happened in earlier scanline
        scanline_render_cycle = (scanline + 15);
    }

    auto data_access_cycle = scanline_render_cycle % 8;

    // which of 8 rows witin a tile
    uint8_t tile_row_index = (cur_scanline + regs.yScroll) % 8;

    if (data_access_cycle == 0)
    {
        // fetch nametable byte for current 8-pixel-tile
        // http://wiki.nesdev.com/w/index.php/PPU_nametables
        uint16_t name_tbl_addr = (*regs.PPUAddr & 0xfff) | 0x2000;
        tileIndex = (*sharedMemory)[name_tbl_addr];
    }
    else if (data_access_cycle == 2)
    {
        // fetch attribute table byte
        // each attribute pixel is 4 quadrant of 2x2 tile (so total of 8x8) tile
        // the result color byte is 2-bit (bit 3/2) for each quadrant
        // http://wiki.nesdev.com/w/index.php/PPU_attribute_tables
        // http://wiki.nesdev.com/w/index.php/PPU_scrolling#Wrapping_around
        uint8_t tile_column = *regs.PPUAddr & 0x1f;         // YY YYYX XXXX = 1 1111
        uint8_t tile_row = (*regs.PPUAddr & 0x3e0) >> 5;    // YY YYYX XXXX = 11 1110 0000
        uint8_t tile_attr_column = (tile_column >> 2) & 0x7;
        uint8_t tile_attr_row = (tile_row >> 2) & 0x7;
        uint16_t attr_tbl_addr = 0x23c0 | (*regs.PPUAddr & 0x0c00) | (tile_attr_row << 3) | tile_attr_column;
        uint8_t color_byte = (*sharedMemory)[attr_tbl_addr];

        // each quadrant has 2x2 tile and each row/column has 4 tiles, so divide by 2 (& 0x2 is faster)
        uint8_t _quadrant_id = (tile_row & 0x2) + ((tile_column & 0x2) >> 1);
        uint8_t color_bit32 = (color_byte & (0x3 << (_quadrant_id * 2))) >> (_quadrant_id * 2);
        tilePaletteBit32 = color_bit32 << 2;
    }
    else if (data_access_cycle == 4)
    {
        // Pattern table is area of memory define all the tiles make up background and sprites.
        // Think it as "lego blocks" that you can build up your background and sprites which
        // simply consists of indexes. It is quite convoluted by today's standards but it is
        // just a space saving technique.
        // http://wiki.nesdev.com/w/index.php/PPU_pattern_tables
        bitPlane0 = read_pattern_table_column(/* sprite = */false, tileIndex, /* bitplane = */ 0, tile_row_index);
    }
    else if (data_access_cycle == 6)
    {
        // fetch tilebitmap high
        // add one more cycle for memory access to skip directly to next access
        uint8_t bitplane1 = read_pattern_table_column(/* sprite = */false, tileIndex, /* bitplane = */ 1, tile_row_index);

        // for each column - bitplane0/bitplane1 has entire 8 column
        // high bit -> low bit
        int start_bit = 7;
        int end_bit = 0;

        int tile = (int)(scanline_render_cycle - /* current_access_cycle */ 6) / 8;
        if (regs.X > 0)
        {
            if (tile == 0)
            {
                start_bit = 7 - regs.X;
            }
            else if (tile == 32)
            {
                // last tile
                end_bit = 7 - regs.X + 1;
            }
            else if (tile > 32)
            {
                // no need to render more than 33 tiles
                // otherwise you'll see wrapped tiles in the begining of next line
                return;
            }
        }
        else
        {
            // We render exactly 32 tiles
            if (tile > 31) return;
        }

        for (int i = start_bit; i >= end_bit; --i)
        {
            uint8_t column_mask = 1 << i;
            uint8_t tile_palette_bit01 = ((bitPlane0 & column_mask) >> i) | ((bitplane1 & column_mask) >> i << 1);
            uint8_t color_4_bit = tilePaletteBit32 | tile_palette_bit01;

            _pixel_cycle[i] = get_palette_color(/* is_background = */ true, color_4_bit);

            uint16_t frame_addr = uint16_t(cur_scanline) * resolution.x + xOffset++;
            if (frame_addr >= frameBuffer1.size())
                continue;
            entireFrameBuffer[frame_addr] = _pixel_cycle[i];

            // record the palette index just for sprite 0 hit detection
            // the detection use palette 0 instead of actual color
            frameBufferBG[frame_addr] = tile_palette_bit01;
        }

        // Increment X position
        if ((*regs.PPUAddr & 0x1f) == 0x1f)
        {
            // Wrap to the next name table
            *regs.PPUAddr &= ~0x1f;
            *regs.PPUAddr ^= 0x0400;
        }
        else
        {
            *regs.PPUAddr++;
        }
    }
}

void PPU::fetch_sprite(uint8_t sprite_id) {
    assert(sprite_id < 8);

    Sprite *sprite = &spriteBuffer[sprite_id];
    uint8_t tile_index = sprite->tileIndex;

    u8 spriteHeight = regs.use8x16Sprites() ? 16 : 8;

    uint8_t tile_row_index = (scanline - sprite->y - 1) % spriteHeight;

    //check vertical flip
    if (sprite->attribute & Bit7)
        tile_row_index = spriteHeight - 1 - tile_row_index;

    uint8_t bitplane0, bitplane1;
    if (regs.use8x16Sprites())
    {
        bitplane0 = read_pattern_table_column_8x16_sprite(tile_index, 0, tile_row_index);
        bitplane1 = read_pattern_table_column_8x16_sprite(tile_index, 1, tile_row_index);
    }
    else
    {
        bitplane0 = read_pattern_table_column(/* sprite = */ true, tile_index, 0, tile_row_index);
        bitplane1 = read_pattern_table_column(/* sprite = */ true, tile_index, 1, tile_row_index);
    }

    // bit3/2 is shared for the entire sprite (just like background attribute table)
    uint8_t palette_index_bit32 = (sprite->attribute & 0x3) << 2;

    // loop all bits - high -> low
    for (int i = 7; i >= 0; --i)
    {
        uint8_t column_mask = 1 << i;
        uint8_t palette_index_bit01 = ((bitplane1 & column_mask) >> i << 1) | ((bitplane0 & column_mask) >> i);

        // palette 0 is always background
        if (palette_index_bit01 == 0)
            continue;

        uint8_t palette_index = palette_index_bit32 | palette_index_bit01;

        uint8_t color = get_palette_color(/* is_background = */false, palette_index);
        uint16_t frame_addr = scanline * resolution.x + sprite->x;
        if (sprite->attribute & Bit6)
            frame_addr += i;     // low -> high in horizontal flip
        else
            frame_addr += 7 - i; // high -> low as usual

        if (frame_addr >= sizeof(frameBuffer1))
        {
            // part of the sprite might be over
            continue;
        }

        bool is_sprite_0 = (hasSprite0 && sprite_id == 0);
        bool behind_bg = sprite->attribute & Bit5;
        if (behind_bg || is_sprite_0)
        {
            // use the recorded 2-bit palette index for sprite 0 hit detection
            // don't use the actual color as some times game use all 0f 'black' palette to black out screen
            bool overlap = (frameBufferBG[frame_addr] != 0);
            if (overlap)
            {
                if (is_sprite_0)
                {
                    // sprite 0 hit detection
                    regs.setSprite0Hit(true);
                }

                if (behind_bg)
                {
                    // behind background
                    continue;
                }
            }
        }

        entireFrameBuffer[frame_addr] = color;
    }
}

u8 PPU::read_pattern_table_column(bool sprite, u8 tile_index, u8 bitplane, u8 tile_row_index) {
    uint16_t tile_addr = sprite ? regs.spritePatternTableAddress() : regs.backgroundPatternTableAddress();
    tile_addr |= (tile_index << 4);

    return (*sharedMemory)[tile_addr | (bitplane << 3) | tile_row_index];
}

uint8_t PPU::read_pattern_table_column_8x16_sprite(uint8_t tile_index, uint8_t bitplane, uint8_t tile_row_index) {
    // TTTTTTB - T is tile number and B is tile pattern table select $0000 or $1000
    uint16_t tile_addr = ((uint16_t(tile_index) & 0x1) << 12) | ((uint16_t(tile_index) & ~0x1) << 4);

    // the tiles are laid like this:
    // 0-7: bitplane 0 for top tile       --> tile row index 0-7
    // 8-f: bitplane 1 for top tile       --> tile row index 0-7
    // 10-17: bitplane 0 for bottom tile  --> tile row index 8-f
    // 18-1f: bitplane 1 for bottom tile  --> tile row index 8-f
    return (*sharedMemory)[tile_addr | (bitplane << 3) | (tile_row_index & 0x7) | ((tile_row_index & 0x8) << 1)];
}

u8 PPU::get_palette_color(bool is_background, uint8_t palette_index_4_bit) {
    // There is only one universal backdrop color doesn't matter which background it is
    if ((palette_index_4_bit & 0x3) == 0)
        return (*sharedMemory)[0x3f00];

    uint16_t palette_addr = (is_background ? 0x3f00 : 0x3f10) | palette_index_4_bit;
    return (*sharedMemory)[palette_addr];
}

Sprite* PPU::getSprite(uint8_t sprite_id) {
    return (Sprite *)oam[sprite_id];
}
