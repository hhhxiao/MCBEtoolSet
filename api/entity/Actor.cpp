//
// Created by xhy on 2020/8/25.
//

#include "Actor.h"
#include "Dimension.h"
#include "Offset.h"
#include "block/BlockSource.h"
#include "lib/mod.h"
#include "tools/CastHelper.h"
#include "tools/MsgBuilder.h"
#include "world/Biome.h"
#include <bitset>

namespace trapdoor {

using namespace SymHook;

uint64_t NetworkIdentifier::getHash() {
    return SYM_CALL(
        uint64_t(*)(NetworkIdentifier *),
        SymHook::MSSYM_B1QA7getHashB1AE17NetworkIdentifierB2AAA4QEBAB1UA3KXZ,
        this);
}

Vec3 *Actor::getPos() {
    return SYM_CALL(
        Vec3 * (*)(void *),
        SymHook::MSSYM_B1QA6getPosB1AA5ActorB2AAE12UEBAAEBVVec3B2AAA2XZ, this);
}

void Actor::getViewActor(Vec3 *vec3, float val) {
    SYM_CALL(
        Vec3 * (*)(Actor *, Vec3 *, float),
        SymHook::
            MSSYM_B1QE13getViewVectorB1AA5ActorB2AAA4QEBAB1QA6AVVec3B2AAA1MB1AA1Z,
        this, vec3, val);
}

std::string Actor::getNameTag() {
    return *SYM_CALL(
        std::string * (*)(Actor *),
        SymHook::
            MSSYM_B1QE10getNameTagB1AA5ActorB2AAA8UEBAAEBVB2QDA5basicB1UA6stringB1AA2DUB2QDA4charB1UA6traitsB1AA1DB1AA3stdB2AAA1VB2QDA9allocatorB1AA1DB1AA12B2AAA3stdB2AAA2XZ,
        this);
}

BlockSource *Actor::getBlockSource() {
    //! from Player::tickWorld
    //  return offset_cast<BlockSource *>(this, 100);
    return *((struct BlockSource **)this + off::PLAYER_GET_BLOCKSOURCE);
}

void Actor::setGameMode(int mode) {
    SYM_CALL(
        void (*)(Actor *, int),
        SymHook::
            MSSYM_B1QE17setPlayerGameTypeB1AE12ServerPlayerB2AAE15UEAAXW4GameTypeB3AAAA1Z,
        this, mode);
}

int Actor::getDimensionID() {
    return *offset_cast<int *>(this, off::ACTOR_GET_DIMENSION_ID);
}

Dimension *Actor::getDimension() {
    return this->getLevel()->getDimFromID(this->getDimensionID());
}

std::string Actor::getDimensionName() {
    auto id = this->getDimensionID();
    if (id == 0)
        return "Overworld";
    if (id == 1)
        return "Nether";
    if (id == 2)
        return "The end";
    return "Unknown";
}

NetworkIdentifier *Actor::getClientID() {
    //! from  ServerPlayer::isHostingPlayer
    return offset_cast<NetworkIdentifier *>(this, off::ACTOR_GET_CLIENT_ID);
}

PlayerPermissionLevel Actor::getCommandLevel() {
    return SYM_CALL(
        PlayerPermissionLevel(*)(Actor *),
        SymHook::
            MSSYM_B1QE25getCommandPermissionLevelB1AA6PlayerB2AAA4UEBAB1QE25AW4CommandPermissionLevelB2AAA2XZ,
        this);
}

PlayerInventory *Actor::getPlayerInventory() {
    return SYM_CALL(
        PlayerInventory * (*)(Actor *),
        SymHook::
            MSSYM_B1QE11getSuppliesB1AA6PlayerB2AAE23QEAAAEAVPlayerInventoryB2AAA2XZ,
        this);
}

// from: Actor::getLevel
Level *Actor::getLevel() { // NOLINT
    return bdsMod->getLevel();
    // return *reinterpret_cast<trapdoor::Level **>((VA) this + 816);
}

std::string Actor::getActorId() {
    std::vector<std::string> info;
    SYM_CALL(void (*)(void *, std::vector<std::string> &),
             SymHook::MSSYM_MD5_f04fad6bac034f1e861181d3580320f2, this, info);
    if (info.empty())
        return "null";
    std::string name = info[0];
    name.erase(0, 23); // remove:Entity:minecraft
    name.pop_back();
    name.pop_back(); // remove <>
    return name;
}

BlockPos Actor::getStandPosition() {
    auto headPos = this->getPos()->toBlockPos();
    return {headPos.x, headPos.y - 2, headPos.z};
}

void Actor::setNameTag(const std::string &name) {
    SYM_CALL(
        void (*)(void *actor, const std::string &str),
        MSSYM_B1QE10setNameTagB1AA5ActorB2AAA9UEAAXAEBVB2QDA5basicB1UA6stringB1AA2DUB2QDA4charB1UA6traitsB1AA1DB1AA3stdB2AAA1VB2QDA9allocatorB1AA1DB1AA12B2AAA3stdB3AAAA1Z,
        this, name);
}

std::string ActorDefinitionIdentifier::getName() {
    auto str = offset_cast<std::string *>(this, off::ACTOR_ID_GET_NAME);
    return std::string(*str);
}

} // namespace trapdoor

using namespace SymHook;

// THook(void, MSSYM_B1QA6attackB1AA6PlayerB2AAA4UEAAB1UE10NAEAVActorB3AAAA1Z,
//       trapdoor::Actor *p1, trapdoor::Actor *p2) {
//     if (p2) {
//         auto result = trapdoor::bdsMod->attackEntityHook(p1, p2);
//         if (result) {
//             original(p1, p2);
//         }
//     } else {
//         original(p1, p2);
//     }
// }
// spawn mob
