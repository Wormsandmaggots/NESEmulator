//
// Created by Radek on 30.12.2024.
//

#include "PPU.h"

#include <cassert>
#include <cstring>

#include "Cartridge.h"
#include "CPU.h"

#define PPU_SCANLINE_CYCLE nes_ppu_cycle_t(341)
#define PPU_SCANLINE_COUNT 262

std::function<void(u16)> PPU::setOAMDMA = nullptr;


namespace ppu {
    constexpr u16 PPUCTRLAddress = 0x2000;
    constexpr u16 PPUMASKAddress = 0x2001;
    constexpr u16 PPUSTATUSAddress = 0x2002;
    constexpr u16 OAMADDRAddress = 0x2003;
    constexpr u16 OAMDATAAddress = 0x2004;
    constexpr u16 PPUSCROLLAddress = 0x2005;
    constexpr u16 PPUADDRAddress = 0x2006;
    constexpr u16 PPUDATAAddress = 0x2007;
    constexpr u16 OAMDMAAddress = 0x4014;
}

bool isIOReg(u16 addr) {
    if ((addr & 0xfff8) == 0x2000)
        return true;
    // $4000~401f
    if ((addr & 0xffe0) == 0x4000)
        return true;
    return false;
}

using namespace ppu;

PPU::PPU(Memory* shared) {
    this->sharedMemory = shared;
    init(sharedMemory);

    setOAMDMA = std::function<void(u16)>([&](u16 address) {
        if (*regs.OAMAddr == 0)
        {
            sharedMemory->get_bytes(regs.oam.data(), 0x100, address, 0x100);
        }
        else
        {
            int copy_before_wrap = 0x100 - *regs.OAMAddr;
            sharedMemory->get_bytes(regs.oam.data() + *regs.OAMAddr, copy_before_wrap, address, copy_before_wrap);
            sharedMemory->get_bytes(regs.oam.data(), 0x100 - copy_before_wrap, address + copy_before_wrap, 0x100 - copy_before_wrap);
        }
    });

    sharedMemory->beforeWrite.push_back([this](u16 addr, u8& val) -> bool {
        switch (addr) {
            case PPUCTRLAddress:
                if(!is_ready()) return false;

                regs.writePPUCTRL(val);
                return false;
            case PPUMASKAddress:
                regs.writePPUMASK(val);
                return false;
            case OAMADDRAddress:
                regs.writeOAMADDR(val);
                return false;
            case OAMDATAAddress:
                regs.writeOAMDATA(val);
                return false;
            case PPUSCROLLAddress:
                if(!is_ready()) return false;

                regs.writePPUSCROLL(val);
                return false;
            case PPUADDRAddress:
                regs.writePPUAADDR(val);
                return false;
            case PPUDATAAddress:
                regs.writePPUData(val, this);
                return false;
            case OAMDMAAddress:
                regs.writeOAMDMA(val);
                return false;
            default:
                //do nothing
                break;
        }

        if(isIOReg(addr) && addr != 0x4016 && addr != 4017) regs.writeLatch(val);

        return true;
    });

    sharedMemory->beforeRead.push_back([this](u16 addr) -> std::optional<u8> {

        switch (addr) {
            case PPUSTATUSAddress:
                return regs.readPPUSTATUS();
            case OAMDATAAddress:
                return regs.readOAMDATA();
            case PPUDATAAddress:
                return regs.readPPUDATA(this);
            default:
                //do nothing
                break;
        }

        const bool returnLatchCondition = isIOReg(addr) && addr != input::p1 && addr != input::p2;

        return returnLatchCondition ? std::optional(regs.latch) : std::nullopt;
    });

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
    regs.PPUControl = &shared->getReference(PPUCTRLAddress);
    regs.PPUData = &shared->getReference(PPUDATAAddress);
    regs.PPUMask = &shared->getReference(PPUMASKAddress);
    regs.PPUScroll = &shared->getReference(PPUSCROLLAddress);
    regs.PPUStatus = &shared->getReference(PPUSTATUSAddress);
    regs.OAMDMA = &shared->getReference(OAMDMAAddress);

    entireFrameBuffer = frameBuffer1.data();
    frameBuffer1.resize(resolution.x * resolution.y);
    frameBuffer2.resize(resolution.x * resolution.y);
    frameBufferBG.resize(resolution.x * resolution.y);

    spriteBuffer.resize(8);

    regs.oam.resize(oamSize, 0);
    vram.resize(vRamSize, 0);

    _master_cycle = nes_cycle_t(0);
    _scanline_cycle = nes_ppu_cycle_t(0);
}

std::vector<u8> PPU::getFrameBuffer() const {
    if(entireFrameBuffer == frameBuffer1.data())
        return frameBuffer2;

    return frameBuffer1;
}

std::vector<uint32_t> PPU::getFrame() const {
    std::vector<u32> frame;
    std::vector<u8> frameBuffer = getFrameBuffer();

    frame.resize(frameBuffer.size());
    for (int i = 0; i < frameBuffer.size(); ++i) {
        Color c = palette[frameBuffer[i] & 0xff];
        u32 coloru32 = c.makeu32();

        frame[i] = coloru32;
    }

    return frame;
}

uint8_t PPU::readVram(uint16_t addr) {
    redirectAddress(addr);
    return vram[addr];
}

void PPU::writeVram(uint16_t addr, uint8_t value) {
    redirectAddress(addr);
    vram[addr] = value;
}

void PPU::writeVram(uint16_t addr, uint8_t *src, uint16_t src_size) {
    redirectAddress(addr);
    memcpy_s(vram.data() + addr, vRamSize - addr, src, src_size);
}

uint8_t *PPU::getVRam() {
    return vram.data();
}

void PPU::setMirroring(nes_mapper_flags flags) {
    mirroring = nes_mapper_flags(flags & nes_mapper_flags_mirroring_mask);
}

void PPU::step(nes_cycle_t count) {
    while(_master_cycle < count) {
        step_ppu(nes_ppu_cycle_t(1));

        if(scanline <= 239) {
            tilesPipeline();
            spritesPipeline();
        }
        else if(scanline == 240) {

        }
        else if(scanline < 261) {
            if(scanline == 241 && _scanline_cycle == nes_ppu_cycle_t(1)) {
                regs.setVblankFlag(true);

                if(regs.vblankNmi())
                    CPU::setNMI();
            }

            if (scanline == 260 && _scanline_cycle > nes_ppu_cycle_t(341 - 12)) {
                regs.setVblankFlag(false);
            }
        }
        else {
            if(scanline == 261) {
                if(_scanline_cycle == nes_ppu_cycle_t(0)) {
                    regs.setVblankFlag(false);

                    if (regs.showBackground() || regs.showSprites())
                    {
                        regs.V = regs.T;
                    }
                }
                else if(_scanline_cycle == nes_ppu_cycle_t(1)) {
                    regs.setSprite0Hit(false);
                }
            }

            if(_scanline_cycle == nes_ppu_cycle_t(340) && frameCount % 2 == 1) {
                step_ppu(nes_ppu_cycle_t(1));
            }
        }
    }
}

void PPU::tilesPipeline() {
    if (!regs.showBackground()) return;

    if (_scanline_cycle == nes_ppu_cycle_t(0))
    {
    }
    else if (_scanline_cycle < nes_ppu_cycle_t(257))
    {
        fetchTile();

        if (_scanline_cycle == nes_ppu_cycle_t(256))
        {
            if ((regs.V & 0x7000) != 0x7000)
            {
                regs.V += 0x1000;
            }
            else
            {
                regs.V &= ~0x7000;

                if ((regs.V & 0x3e0) != 0x3a0)
                {
                    regs.V += 0x20;
                }
                else
                {
                    regs.V &= ~0x3e0;

                    regs.V ^= 0x0800;
                }
            }
        }
    }
    else if (_scanline_cycle < nes_ppu_cycle_t(321))
    {
        if (_scanline_cycle == nes_ppu_cycle_t(257))
        {
            regs.V = (regs.V & 0xfbe0) | (regs.T & ~0xfbe0);
            xOffset = 0;
        }
    }
    else if (_scanline_cycle < nes_ppu_cycle_t(337))
    {
        fetchTile();
    }
    else
    {
    }
}

void PPU::spritesPipeline() {
    if (!regs.showSprites())
        return;

    if (scanline == 0)
        return;

    if (_scanline_cycle == nes_ppu_cycle_t(0))
    {
        lastSpriteID = 0;
        hasSprite0 = false;
        memset(spriteBuffer.data(), 0xff, spriteBuffer.size() * sizeof(Sprite));
        regs.setSpriteOverflow(false);
    }
    else if (_scanline_cycle < nes_ppu_cycle_t(65))
    {
        regs.maskOAMRead = true;
    }
    else if (_scanline_cycle < nes_ppu_cycle_t(257))
    {
        regs.maskOAMRead = false;

        u8 sprite_cycle = (uint8_t) (_scanline_cycle.count() - 65);
        u8 sprite_id = sprite_cycle / 2;

        if (sprite_id >= 64)
            return;

        if ((_scanline_cycle.count() % 2) == 0)
        {
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
            Sprite* sprite = getSprite(sprite_id);
            lastSpriteY = sprite->y;
        }
    }
    else if (_scanline_cycle < nes_ppu_cycle_t(321))
    {
        auto sprite_cycle = (u8)(_scanline_cycle.count() - 257);
        u8 sprite_id = sprite_cycle / 8;
        if (sprite_cycle % 8 == 4)
        {
            if (sprite_id < lastSpriteID)
                fetch_sprite(sprite_id);
        }
    }
    else
    {
    }
}

void PPU::fetchTile() {
    auto scanline_render_cycle = nes_ppu_cycle_t(0);
    u16 cur_scanline = scanline;

    if (_scanline_cycle > nes_ppu_cycle_t(320))
    {
        scanline_render_cycle = _scanline_cycle - nes_ppu_cycle_t(321);
        cur_scanline = (cur_scanline + 1) % resolution.y;
    }
    else
    {
        scanline_render_cycle = (_scanline_cycle - nes_ppu_cycle_t(1) + nes_ppu_cycle_t(16));
    }

    auto data_access_cycle = scanline_render_cycle % 8;

    uint8_t tile_row_index = (cur_scanline + regs.yScroll) % 8;

    if (data_access_cycle == nes_ppu_cycle_t(0))
    {
        // http://wiki.nesdev.com/w/index.php/PPU_nametables
        uint16_t name_tbl_addr = (regs.V & 0xfff) | 0x2000;
        tileIndex = readVram(name_tbl_addr);
    }
    else if (data_access_cycle == nes_ppu_cycle_t(2))
    {
        // http://wiki.nesdev.com/w/index.php/PPU_attribute_tables
        // http://wiki.nesdev.com/w/index.php/PPU_scrolling#Wrapping_around
        uint8_t tile_column = regs.V & 0x1f;         // YY YYYX XXXX = 1 1111
        uint8_t tile_row = (regs.V & 0x3e0) >> 5;    // YY YYYX XXXX = 11 1110 0000
        uint8_t tile_attr_column = (tile_column >> 2) & 0x7;
        uint8_t tile_attr_row = (tile_row >> 2) & 0x7;
        uint16_t attr_tbl_addr = 0x23c0 | (regs.V & 0x0c00) | (tile_attr_row << 3) | tile_attr_column;
        uint8_t color_byte = readVram(attr_tbl_addr);

        uint8_t _quadrant_id = (tile_row & 0x2) + ((tile_column & 0x2) >> 1);
        uint8_t color_bit32 = (color_byte & (0x3 << (_quadrant_id * 2))) >> (_quadrant_id * 2);
        tilePaletteBit32 = color_bit32 << 2;
    }
    else if (data_access_cycle == nes_ppu_cycle_t(4))
    {
        // http://wiki.nesdev.com/w/index.php/PPU_pattern_tables
        bitPlane0 = read_pattern_table_column(/* sprite = */false, tileIndex, /* bitplane = */ 0, tile_row_index);
    }
    else if (data_access_cycle == nes_ppu_cycle_t(6))
    {
        uint8_t bitplane1 = read_pattern_table_column(/* sprite = */false, tileIndex, /* bitplane = */ 1, tile_row_index);

        int start_bit = 7;
        int end_bit = 0;

        int tile = (int)(scanline_render_cycle.count() - /* current_access_cycle */ 6) / 8;
        if (regs.X > 0)
        {
            if (tile == 0)
            {
                start_bit = 7 - regs.X;
            }
            else if (tile == 32)
            {
                end_bit = 7 - regs.X + 1;
            }
            else if (tile > 32)
            {
                return;
            }
        }
        else
        {
            if (tile > 31) return;
        }

        for (int i = start_bit; i >= end_bit; --i)
        {
            uint8_t column_mask = 1 << i;
            uint8_t tile_palette_bit01 = ((bitPlane0 & column_mask) >> i) | ((bitplane1 & column_mask) >> i << 1);
            uint8_t color_4_bit = tilePaletteBit32 | tile_palette_bit01;

            _pixel_cycle[i] = get_palette_color(/* is_background = */ true, color_4_bit);

            uint16_t frame_addr = u16(cur_scanline) * resolution.x + xOffset++;

            if (frame_addr >= frameBuffer1.size())
                continue;
            entireFrameBuffer[frame_addr] = _pixel_cycle[i];

            frameBufferBG[frame_addr] = tile_palette_bit01;
        }

        if ((regs.V & 0x1f) == 0x1f)
        {
            regs.V &= ~0x1f;
            regs.V ^= 0x0400;
        }
        else
        {
            regs.V++;
        }
    }
}

void PPU::fetch_sprite(uint8_t sprite_id) {
    assert(sprite_id < 8);

    Sprite *sprite = &spriteBuffer[sprite_id];
    uint8_t tile_index = sprite->tileIndex;

    u8 spriteHeight = regs.use8x16Sprites() ? 16 : 8;

    uint8_t tile_row_index = (scanline - sprite->y - 1) % spriteHeight;

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

    uint8_t palette_index_bit32 = (sprite->attribute & 0x3) << 2;

    for (int i = 7; i >= 0; --i)
    {
        uint8_t column_mask = 1 << i;
        uint8_t palette_index_bit01 = ((bitplane1 & column_mask) >> i << 1) | ((bitplane0 & column_mask) >> i);

        if (palette_index_bit01 == 0)
            continue;

        uint8_t palette_index = palette_index_bit32 | palette_index_bit01;

        uint8_t color = get_palette_color(/* is_background = */false, palette_index);
        uint16_t frame_addr = scanline * resolution.x + sprite->x;
        if (sprite->attribute & Bit6)
            frame_addr += i;
        else
            frame_addr += 7 - i;

        if (frame_addr >= frameBuffer1.size())
        {
            continue;
        }

        bool is_sprite_0 = (hasSprite0 && sprite_id == 0);
        bool behind_bg = sprite->attribute & Bit5;
        if (behind_bg || is_sprite_0)
        {
            bool overlap = (frameBufferBG[frame_addr] != 0);
            if (overlap)
            {
                if (is_sprite_0)
                {
                    regs.setSprite0Hit(true);
                }

                if (behind_bg)
                {
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

    return readVram(tile_addr | (bitplane << 3) | tile_row_index);
}

uint8_t PPU::read_pattern_table_column_8x16_sprite(uint8_t tile_index, uint8_t bitplane, uint8_t tile_row_index) {
    uint16_t tile_addr = ((uint16_t(tile_index) & 0x1) << 12) | ((uint16_t(tile_index) & ~0x1) << 4);

    return readVram(tile_addr | (bitplane << 3) | (tile_row_index & 0x7) | ((tile_row_index & 0x8) << 1));
}

u8 PPU::get_palette_color(bool is_background, uint8_t palette_index_4_bit) {
    if ((palette_index_4_bit & 0x3) == 0)
        return readVram(0x3f00);

    uint16_t palette_addr = (is_background ? 0x3f00 : 0x3f10) | palette_index_4_bit;
    return readVram(palette_addr);
}

Sprite* PPU::getSprite(uint8_t sprite_id) const {
    return &((Sprite *)regs.oam.data())[sprite_id];;
}

void PPU::step_ppu(nes_ppu_cycle_t count) {
    _master_cycle += nes_ppu_cycle_t(count);
    _scanline_cycle += nes_ppu_cycle_t(count);

    if (_scanline_cycle >= PPU_SCANLINE_CYCLE)
    {
        _scanline_cycle %= PPU_SCANLINE_CYCLE;
        scanline++;
        if (scanline >= PPU_SCANLINE_COUNT)
        {
            scanline %= PPU_SCANLINE_COUNT;
            swap_buffer();
            frameCount++;
        }
    }
}

void PPU::swap_buffer() {
    if (entireFrameBuffer == frameBuffer1.data())
        entireFrameBuffer = frameBuffer2.data();
    else
        entireFrameBuffer = frameBuffer1.data();
}

void PPU::redirectAddress(uint16_t &addr) {
    if ((addr & 0xff00) == 0x3f00)
    {
        addr &= 0xff1f;

        if ((addr & 0xfff3) == 0x3f10)
            addr &= 0x3f0f;
    }
    else if ((addr & 0xf000) == 0x2000)
    {
        if (mirroring == nes_mapper_flags_vertical_mirroring)
            addr &= 0xf7ff;     // $2000=$2800, $2400=$2c00
        else if (mirroring == nes_mapper_flags_horizontal_mirroring)
            addr &= 0xfbff;     // $2000=$2400, $2800=$2c00
        else assert(!"Unsupported mirroring modes");
    }
    else if (addr >= 0x3000)
    {
        addr -= 0x1000;
    }
}
