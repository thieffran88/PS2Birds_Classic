-- puffer_bird.lua (GlobeBird specialty), recovered from the supplied Classic APK and decompiled with unluac; read only.
local L0_1, L1_1
function L0_1(A0_2)
  local L1_2, L2_2, L3_2, L4_2, L5_2, L6_2
  L1_2 = A0_2.isPuffing
  if L1_2 then
    return
  end
  L1_2 = A0_2.pufferDefinition
  if not L1_2 then
    L1_2 = blockTable
    L1_2 = L1_2.blocks
    L2_2 = A0_2.definition
    L1_2 = L1_2[L2_2]
  end
  L2_2 = hasObjectUpdateFunction
  L3_2 = A0_2
  L4_2 = pufferDelayTimer
  L2_2 = L2_2(L3_2, L4_2)
  if L2_2 then
    L2_2 = removeObjectUpdateFunction
    L3_2 = A0_2
    L4_2 = pufferDelayTimer
    L2_2(L3_2, L4_2)
  end
  L2_2 = L1_2.puffTime
  A0_2.puffTimer = L2_2
  L2_2 = setFriction
  L3_2 = A0_2.name
  L4_2 = L1_2.puffedValues
  L4_2 = L4_2.friction
  L2_2(L3_2, L4_2)
  L2_2 = setRestitution
  L3_2 = A0_2.name
  L4_2 = L1_2.puffedValues
  L4_2 = L4_2.restitution
  L2_2(L3_2, L4_2)
  L2_2 = setDensity
  L3_2 = A0_2.name
  L4_2 = L1_2.puffedValues
  L4_2 = L4_2.density
  L2_2(L3_2, L4_2)
  L2_2 = L1_2.puffedValues
  L2_2 = L2_2.damageFactors
  A0_2.damageFactors = L2_2
  L2_2 = L1_2.spriteSpecial
  A0_2.sprite = L2_2
  L2_2 = setSprite
  L3_2 = A0_2.name
  L4_2 = L1_2.spriteSpecial
  L2_2(L3_2, L4_2)
  L2_2 = resizeCircle
  L3_2 = A0_2.name
  L4_2 = L1_2.puffedValues
  L4_2 = L4_2.radius
  L5_2 = A0_2.radius
  L6_2 = L1_2.radius
  L5_2 = L5_2 / L6_2
  L4_2 = L4_2 * L5_2
  L2_2(L3_2, L4_2)
  L2_2 = makeExplosion
  L3_2 = A0_2
  L4_2 = getObjectDefinition
  L5_2 = A0_2.name
  L4_2 = L4_2(L5_2)
  L5_2 = nil
  L2_2(L3_2, L4_2, L5_2)
  L2_2 = addObjectUpdateFunction
  L3_2 = A0_2
  L4_2 = pufferTimer
  L2_2(L3_2, L4_2)
  L2_2 = playAudio
  L3_2 = getAudioName
  L4_2 = L1_2.specialSound
  L3_2 = L3_2(L4_2)
  L4_2 = 1
  L5_2 = false
  L2_2(L3_2, L4_2, L5_2)
  A0_2.isPuffing = true
end
triggerPufferSpecialty = L0_1
function L0_1(A0_2)
  local L1_2, L2_2, L3_2, L4_2
  L1_2 = A0_2.specialtyTriggered
  if not L1_2 then
    L1_2 = A0_2.pufferDefinition
    if not L1_2 then
      L1_2 = blockTable
      L1_2 = L1_2.blocks
      L2_2 = A0_2.definition
      L1_2 = L1_2[L2_2]
    end
    L2_2 = L1_2.automaticPuffDelay
    A0_2.puffTimer = L2_2
    L2_2 = addObjectUpdateFunction
    L3_2 = A0_2
    L4_2 = pufferDelayTimer
    L2_2(L3_2, L4_2)
  end
end
flyingPufferCollided = L0_1
function L0_1(A0_2, A1_2)
  local L2_2, L3_2, L4_2
  L2_2 = A0_2.puffTimer
  L2_2 = L2_2 - A1_2
  A0_2.puffTimer = L2_2
  L2_2 = A0_2.puffTimer
  if L2_2 <= 0 then
    L2_2 = triggerSpecialty
    L3_2 = A0_2
    L4_2 = A1_2
    L2_2(L3_2, L4_2)
  end
end
pufferDelayTimer = L0_1
function L0_1(A0_2, A1_2)
  local L2_2, L3_2, L4_2, L5_2, L6_2
  A0_2.strength = 100000
  L2_2 = A0_2.puffTimer
  L2_2 = L2_2 - A1_2
  A0_2.puffTimer = L2_2
  L2_2 = A0_2.puffTimer
  if L2_2 <= 0 then
    A0_2.puffTimer = nil
    L2_2 = removeObjectUpdateFunction
    L3_2 = A0_2
    L4_2 = pufferTimer
    L2_2(L3_2, L4_2)
    L2_2 = A0_2.pufferDefinition
    if not L2_2 then
      L2_2 = blockTable
      L2_2 = L2_2.blocks
      L3_2 = A0_2.definition
      L2_2 = L2_2[L3_2]
    end
    L3_2 = L2_2.deathTime
    A0_2.deathTimer = L3_2
    L3_2 = L2_2.directionChangeTime
    A0_2.directionChangeTimer = L3_2
    L3_2 = addObjectUpdateFunction
    L4_2 = A0_2
    L5_2 = pufferDeath
    L3_2(L4_2, L5_2)
    L3_2 = playAudio
    L4_2 = getAudioName
    L5_2 = L2_2.removeSound
    L4_2 = L4_2(L5_2)
    L5_2 = 1
    L6_2 = false
    L3_2(L4_2, L5_2, L6_2)
  end
end
pufferTimer = L0_1
function L0_1(A0_2, A1_2)
  local L2_2, L3_2, L4_2, L5_2, L6_2, L7_2
  L2_2 = A0_2.pufferDefinition
  if not L2_2 then
    L2_2 = blockTable
    L2_2 = L2_2.blocks
    L3_2 = A0_2.definition
    L2_2 = L2_2[L3_2]
  end
  A0_2.strength = 100000
  L3_2 = A0_2.directionChangeTimer
  L3_2 = L3_2 - A1_2
  A0_2.directionChangeTimer = L3_2
  L3_2 = A0_2.directionChangeTimer
  if L3_2 <= 0 then
    L3_2 = A0_2.xVel
    L4_2 = _G
    L4_2 = L4_2.math
    L4_2 = L4_2.random
    L5_2 = L2_2.directionChangeMagnitude
    L5_2 = -L5_2
    L6_2 = L2_2.directionChangeMagnitude
    L4_2 = L4_2(L5_2, L6_2)
    L3_2 = L3_2 + L4_2
    A0_2.xVel = L3_2
    L3_2 = A0_2.yVel
    L4_2 = _G
    L4_2 = L4_2.math
    L4_2 = L4_2.random
    L5_2 = L2_2.directionChangeMagnitude
    L5_2 = -L5_2
    L6_2 = L2_2.directionChangeMagnitude
    L4_2 = L4_2(L5_2, L6_2)
    L3_2 = L3_2 + L4_2
    A0_2.yVel = L3_2
    L3_2 = setRotation
    L4_2 = A0_2.name
    L5_2 = _G
    L5_2 = L5_2.math
    L5_2 = L5_2.atan2
    L6_2 = A0_2.xVel
    L6_2 = -L6_2
    L7_2 = A0_2.yVel
    L5_2 = L5_2(L6_2, L7_2)
    L6_2 = _G
    L6_2 = L6_2.math
    L6_2 = L6_2.pi
    L6_2 = L6_2 * 0.5
    L5_2 = L5_2 - L6_2
    L3_2(L4_2, L5_2)
    L3_2 = setVelocity
    L4_2 = A0_2.name
    L5_2 = A0_2.xVel
    L6_2 = A0_2.yVel
    L3_2(L4_2, L5_2, L6_2)
    L3_2 = L2_2.directionChangeTime
    A0_2.directionChangeTimer = L3_2
  end
  L3_2 = A0_2.deathTimer
  L3_2 = L3_2 - A1_2
  A0_2.deathTimer = L3_2
  L3_2 = A0_2.deathTimer
  if L3_2 <= 0 then
    A0_2.deflated = true
    L3_2 = contains
    L4_2 = birds
    L5_2 = A0_2
    L3_2 = L3_2(L4_2, L5_2)
    if L3_2 then
      L3_2 = removeBird
      L4_2 = A0_2
      L3_2(L4_2)
    else
      L3_2 = deadBlocks
      L4_2 = A0_2.name
      L3_2[L4_2] = A0_2
    end
  else
    L3_2 = A0_2.deathTimer
    L4_2 = L2_2.deathTime
    L3_2 = L3_2 / L4_2
    L4_2 = resizeCircle
    L5_2 = A0_2.name
    L6_2 = L2_2.puffedValues
    L6_2 = L6_2.radius
    L6_2 = L6_2 * L3_2
    L4_2(L5_2, L6_2)
    L4_2 = setScale
    L5_2 = A0_2.name
    L6_2 = L3_2
    L7_2 = L3_2
    L4_2(L5_2, L6_2, L7_2)
  end
end
pufferDeath = L0_1
