//

#include "tileinfo.hpp"

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <sstream>

using namespace std::string_literals;

//=================================================================================
namespace ultima {

    //=================================================================================
    // landtile_t constructor
    //=================================================================================
    //=================================================================================
    landtile_t::landtile_t(std::ifstream &input,bool isHS):landtile_t() {
        if (isHS) {
            input.read(reinterpret_cast<char*>(&flag.value),8);
        }
        else {
            input.read(reinterpret_cast<char*>(&flag.value),4);
        }
        input.read(reinterpret_cast<char*>(&textureid),2);
        auto buffer = std::vector<char>(21,0) ;
        input.read(buffer.data(),20);
        name = buffer.data() ;
    }
    //=================================================================================
    auto landtile_t::save(std::ofstream &output,bool isHS) ->void{
        if (isHS){
            output.write(reinterpret_cast<char*>(&flag.value),8);
        }
        else {
            output.write(reinterpret_cast<char*>(&flag.value),4);
        }
        output.write(reinterpret_cast<char*>(&textureid), 2);
        auto buffer = std::vector<char>(20,0) ;
        auto size = std::min(20,static_cast<int>(name.size()));
        std::copy(name.c_str(),name.c_str()+size,buffer.data());
        output.write(buffer.data(),20);
                                                 
    }
    //*********************************************************************************

    //=================================================================================
    // itemtile_t constructor
    //=================================================================================
    //=================================================================================
    itemtile_t::itemtile_t(std::ifstream &input,bool isHS ) {
        if (isHS) {
            input.read(reinterpret_cast<char*>(&flag.value),8);
        }
        else {
            input.read(reinterpret_cast<char*>(&flag.value),4);
        }
        input.read(reinterpret_cast<char*>(&weight),1);
        input.read(reinterpret_cast<char*>(&quality),1);
        input.read(reinterpret_cast<char*>(&misc_data),2);
        input.read(reinterpret_cast<char*>(&unknown2),1);
        input.read(reinterpret_cast<char*>(&quantity),1);
        input.read(reinterpret_cast<char*>(&animid),2);
        input.read(reinterpret_cast<char*>(&unknown3),1);
        input.read(reinterpret_cast<char*>(&hue),1);
        input.read(reinterpret_cast<char*>(&stacking_offset),2);
        input.read(reinterpret_cast<char*>(&height),1);

        
        auto buffer = std::vector<char>(21,0) ;
        input.read(buffer.data(),20);
        name = buffer.data() ;
    }
    //=================================================================================
    auto itemtile_t::save(std::ofstream &output,bool isHS) ->void{
        if (isHS){
            output.write(reinterpret_cast<char*>(&flag.value),8);
        }
        else {
            output.write(reinterpret_cast<char*>(&flag.value),4);
        }
        output.write(reinterpret_cast<char*>(&weight),1);
        output.write(reinterpret_cast<char*>(&quality),1);
        output.write(reinterpret_cast<char*>(&misc_data),2);
        output.write(reinterpret_cast<char*>(&unknown2),1);
        output.write(reinterpret_cast<char*>(&quantity),1);
        output.write(reinterpret_cast<char*>(&animid),2);
        output.write(reinterpret_cast<char*>(&unknown3),1);
        output.write(reinterpret_cast<char*>(&hue),1);
        output.write(reinterpret_cast<char*>(&stacking_offset),2);
        output.write(reinterpret_cast<char*>(&height),1);
        auto buffer = std::vector<char>(20,0) ;
        auto size = std::min(20,static_cast<int>(name.size()));
        std::copy(name.c_str(),name.c_str()+size,buffer.data());
        output.write(buffer.data(),20);
        
    }

    //=================================================================================
    // tileinfo_t class to access tiledata.mul
    //=================================================================================
    
    //=================================================================================
    auto tileinfo_t::loadLand(std::ifstream &input,bool is_hs) ->void {
        landtiles.clear();
        for (std::uint32_t tileid = 0 ; tileid < 0x4000; ++tileid) {
            // We have to get rid of the header on groups of tileids.
            // However, HS size tileids, have the header for the first "group" on between
            // the first and second tileid
             if (is_hs){
                if( (((tileid & 0x1F) == 0 ) && (tileid > 0)) || (tileid == 1) ) {
                    input.seekg(4,std::ios::cur);
                }
            }
            else {
                if ( (tileid&0x1f) == 0) {
                    input.seekg(4,std::ios::cur);
                }
            }
            auto land = landtile_t(input,is_hs) ;
            landtiles.push_back(land);
            if (!input.good()){
                throw std::runtime_error("Error reading land tile information.");
            }
        }
    }
    //=================================================================================
    auto tileinfo_t::saveLand(std::ofstream &output, bool is_hs) ->void {
        auto filler = std::uint32_t(0) ;
        for (std::uint32_t tileid = 0 ; tileid < 0x4000; ++tileid) {
            
            if (is_hs){
                if( (((tileid & 0x1F) == 0 ) && (tileid > 0)) || (tileid == 1) ) {
                    output.write(reinterpret_cast<char*>(&filler),sizeof(filler));
                }
            }
            else {
                if ( (tileid&0x1f) == 0) {
                    output.write(reinterpret_cast<char*>(&filler),sizeof(filler));
               }
            }
            landtiles.at(tileid).save(output,is_hs);
        }

    }
    //=================================================================================
    auto tileinfo_t::loadItem(std::ifstream &input,bool is_hs) ->void {
        itemtiles.clear() ;
        auto tileid = std::uint32_t(0) ;
        while (input.good() && !input.eof() && (tileid < 0x10000)){
            if ( (tileid &0x1f) == 0){
                input.seekg(4,std::ios::cur);
            }
            auto item = itemtile_t(input,is_hs);
            itemtiles.push_back(item) ;
            if (!input.good()){
                throw std::runtime_error("Error reading item tile information.");
            }
            tileid++ ;
        }
    }
    //=================================================================================
    auto tileinfo_t::saveItem(std::ofstream &output,bool is_hs) ->void {
        auto filler = std::uint32_t(0) ;
        for (std::uint32_t tileid = 0 ; tileid < static_cast<std::uint32_t>(itemtiles.size()); ++tileid) {
            if ( (tileid &0x1f) == 0){
                output.write(reinterpret_cast<char*>(&filler), 4);
            }
            itemtiles.at(tileid).save(output, is_hs);
         }
    }

    //=================================================================================
    tileinfo_t::tileinfo_t(const std::filesystem::path &filepath ) {
        landtiles.reserve(0x4000);
        itemtiles.reserve(0x10000) ;
        
        if (!filepath.empty()){
            if (!load(filepath)) {
                throw std::runtime_error("Unable to open: "s + filepath.string());
            }
        }
        else {
            create();
        }
    }
    //=================================================================================
    auto tileinfo_t::load(const std::filesystem::path &filepath) ->bool {
        // Quick check if the file exists or not
        if (!std::filesystem::exists(filepath)){
            return false ;
        }
        auto size = std::filesystem::file_size(filepath) ;
        auto is_hs = false ;
        if (size >= hs_size){
            is_hs = true ;
        }
        auto input = std::ifstream(filepath.string(),std::ios::binary);
        // Quick check if we can open file or not
        if (!input.is_open()){
            return false ;
        }
        loadLand(input, is_hs);
        loadItem(input, is_hs);
        return true ;
    }
        
    //=================================================================================
    auto tileinfo_t::create() ->void {
        landtiles = std::vector<landtile_t>(0x4000);
        itemtiles = std::vector<itemtile_t>(0x10000);
    }
    
    //=================================================================================
    auto tileinfo_t::land(std::uint32_t tileid) const ->const landtile_t&{
        if (static_cast<size_t>(tileid) < landtiles.size() ){
            return landtiles.at(tileid) ;
        }
        throw std::out_of_range("Exceeds max land tile id: "s + std::to_string(tileid));
    }
    //=================================================================================
    auto tileinfo_t::land(std::uint32_t tileid)  -> landtile_t&{
        if (static_cast<size_t>(tileid) < landtiles.size() ){
            return landtiles.at(tileid) ;
        }
        throw std::out_of_range("Exceeds max land tile id: "s + std::to_string(tileid));

    }
    //=================================================================================
    auto tileinfo_t::item(std::uint32_t tileid) const ->const itemtile_t& {
        if (static_cast<size_t>(tileid) < itemtiles.size() ){
            return itemtiles.at(tileid) ;
        }
        throw std::out_of_range("Exceeds max item tile id: "s + std::to_string(tileid));

    }
    //=================================================================================
    auto tileinfo_t::item(std::uint32_t tileid) ->itemtile_t & {
        if (static_cast<size_t>(tileid) < itemtiles.size() ){
            return itemtiles.at(tileid) ;
        }
        throw std::out_of_range("Exceeds max item tile id: "s + std::to_string(tileid));

    }
    //=================================================================================
    auto tileinfo_t::save(const std::filesystem::path &path,bool is_hs) ->void {
        auto output = std::ofstream(path.string(),std::ios::binary);
        if (!output.is_open()){
            throw std::runtime_error("Unable to open: "s + path.string());
        }
        saveLand(output, is_hs);
        saveItem(output, is_hs);
    }
}


