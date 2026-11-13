-- Excerpts: default_conf.lua (bubble constants), bubbles.lua (triggerPinkBirdSpecialty, updateBubble), mode_MuseumMode.lua (pink timer), CollisionCallbacks.lua (1.5 s after contact), particles/sonicBoom.lua (lifeTime); recovered from the supplied Classic APK; read only.
L0_1 = 50
pigDefenceBoost = L0_1
L0_1 = 1.8
bubbleAntiGravity = L0_1
L0_1 = 2
bubbleAntiGravityTime = L0_1
L0_1 = {}
L1_1 = -3
L2_1 = 3
L0_1[1] = L1_1
L0_1[2] = L2_1
bubbleAntiGravityFloat = L0_1
L0_1 = 100
bubbleJointForce = L0_1
L0_1 = 10
antiGravityMaxVelocity = L0_1
L0_1 = false
local L0_1, L1_1
function L0_1(A0_2, A1_2, A2_2, A3_2)
  local L4_2, L5_2, L6_2, L7_2, L8_2, L9_2, L10_2, L11_2, L12_2, L13_2, L14_2, L15_2, L16_2, L17_2
  if not A3_2 then
    A3_2 = 1
  end
  L4_2 = _G
  L4_2 = L4_2.pairs
  L5_2 = objects
  L5_2 = L5_2.world
  L4_2, L5_2, L6_2 = L4_2(L5_2)
  for L7_2, L8_2 in L4_2, L5_2, L6_2 do
    L9_2 = getObjectDefinition
    L10_2 = L8_2.name
    L9_2 = L9_2(L10_2)
    L10_2 = L8_2.shot
    if L10_2 then
      L10_2 = L9_2.specialty
      L10_2 = L10_2 ~= "PUFFER"
    end
    L11_2 = contains
    L12_2 = {}
    L13_2 = "immovable"
    L14_2 = "staticGround"
    L15_2 = "bubble"
    L12_2[1] = L13_2
    L12_2[2] = L14_2
    L12_2[3] = L15_2
    L13_2 = L9_2.materialName
    L11_2 = L11_2(L12_2, L13_2)
    L11_2 = L9_2.controllable
    L11_2 = not L11_2 and L11_2
    if L11_2 then
      L12_2 = distance
      L13_2 = A0_2.x
      L14_2 = A0_2.y
      L15_2 = L8_2.x
      L16_2 = L8_2.y
      L12_2 = L12_2(L13_2, L14_2, L15_2, L16_2)
      if A1_2 > L12_2 then
        L12_2 = A0_2.name
        if L7_2 ~= L12_2 then
          L8_2.inBubble = true
          L12_2 = addObjectUpdateFunction
          L13_2 = L8_2
          L14_2 = updateBubble
          L12_2(L13_2, L14_2)
          L12_2 = _G
          L12_2 = L12_2.math
          L12_2 = L12_2.random
          L13_2 = 1
          L14_2 = #A2_2
          L12_2 = L12_2(L13_2, L14_2)
          L12_2 = A2_2[L12_2]
          L8_2.bubbleSprite = L12_2
          L8_2.bubbleSpriteScale = A3_2
          L12_2 = bubbleAntiGravityTime
          L8_2.bubbleAntiGravityTimer = L12_2
          L12_2 = L8_2.x
          L13_2 = A0_2.x
          L12_2 = L12_2 - L13_2
          L12_2 = L12_2 * 1.3
          L8_2.bubbleAntiGravityFloat = L12_2
          L12_2 = L8_2.density
          if L12_2 < 0.1 then
            L12_2 = getVelocity
            L13_2 = L8_2.name
            L12_2, L13_2 = L12_2(L13_2)
            L14_2 = setVelocity
            L15_2 = L8_2.name
            L16_2 = L12_2
            L17_2 = 0
            L14_2(L15_2, L16_2, L17_2)
          end
          L12_2 = destroyBreakableJoints
          L13_2 = L8_2.name
          L14_2 = bubbleJointForce
          L12_2(L13_2, L14_2)
        end
      end
    end
  end
  L4_2 = g_extra_draw_functions
  L5_2 = {}
  L6_2 = drawIngameBubbles
  L5_2.func = L6_2
  L5_2.beforeBlocks = false
  L4_2.ingame_bubbles = L5_2
  L4_2 = setVelocity
  L5_2 = A0_2.name
  L6_2 = 0
  L7_2 = 0
  L4_2(L5_2, L6_2, L7_2)
  L4_2 = addObjectUpdateFunction
  L5_2 = A0_2
  L6_2 = updateBubble
  L4_2(L5_2, L6_2)
  A0_2.inBubble = true
  L4_2 = bubbleAntiGravityTime
  A0_2.bubbleAntiGravityTimer = L4_2
  L4_2 = "P_BIRDBUBBLE_"
  L5_2 = _G
  L5_2 = L5_2.math
  L5_2 = L5_2.floor
  L6_2 = _G
  L6_2 = L6_2.math
  L6_2 = L6_2.random
  L7_2 = 1
  L8_2 = 3
  L6_2, L7_2, L8_2, L9_2, L10_2, L11_2, L12_2, L13_2, L14_2, L15_2, L16_2, L17_2 = L6_2(L7_2, L8_2)
      if not L5_2 then
        L5_2 = 0
      end
      L6_2 = A0_2.mass
      L5_2 = L5_2 * L6_2
      L6_2 = objects
      L6_2 = L6_2.worldGravity
      L6_2 = -L6_2
      L7_2 = A0_2.mass
      L6_2 = L6_2 * L7_2
      L7_2 = bubbleAntiGravity
      L6_2 = L6_2 * L7_2
      L7_2 = A0_2.x
      L8_2 = A0_2.y
      L3_2(L4_2, L5_2, L6_2, L7_2, L8_2)
    end
  end
end
function L1_1(A0_2, A1_2)
  local L2_2, L3_2, L4_2, L5_2, L6_2, L7_2, L8_2
  L2_2 = A0_2.inBubble
  if L2_2 then
    L2_2 = A0_2.bubbleAntiGravityTimer
    if 0 < L2_2 then
      L2_2 = A0_2.bubbleAntiGravityTimer
      L3_2 = physicsSpeedFactor
      L3_2 = A1_2 * L3_2
      L2_2 = L2_2 - L3_2
      A0_2.bubbleAntiGravityTimer = L2_2
      L2_2 = L0_1
      L3_2 = A0_2
      L2_2(L3_2)
    else
      A0_2.bubbleAntiGravityTimer = nil
      A0_2.bubbleAntiGravityFloat = nil
      A0_2.inBubble = nil
      L2_2 = removeObjectUpdateFunction
      L3_2 = A0_2
      L4_2 = updateBubble
      L2_2(L3_2, L4_2)
      L2_2 = addParticles
      L3_2 = A0_2.name
      L4_2 = "bubblePop"
      L5_2 = 1
      L6_2 = false
      L7_2 = false
      L8_2 = true
      L2_2(L3_2, L4_2, L5_2, L6_2, L7_2, L8_2)
      L2_2 = A0_2.controllable
      if L2_2 then
        L2_2 = removeBird
        L3_2 = A0_2
        L2_2(L3_2)
      end
    end
  end
end
updateBubble = L1_1
    end
    else
      L4_2 = A2_2.pinkTimer
      if L4_2 ~= nil then
        L4_2 = A2_2.disableTimedSpecialty
        if not L4_2 then
          L4_2 = A2_2.pinkTimer
          L4_2 = L4_2 - A3_2
          A2_2.pinkTimer = L4_2
          L4_2 = A2_2.pinkTimer
          L5_2 = gamelua
          L5_2 = L5_2.particleTable
          L5_2 = L5_2.particles
          L5_2 = L5_2.sonicBoom
          L5_2 = L5_2.lifeTime
          if L4_2 < L5_2 then
            L4_2 = A2_2.sonicBoom
            if not L4_2 then
              A2_2.sonicBoom = true
              L4_2 = gamelua
              L4_2 = L4_2.setVelocity
              L5_2 = A2_2.name
              L6_2 = 0
              L7_2 = 0
              L4_2(L5_2, L6_2, L7_2)
              L4_2 = gamelua
              L4_2 = L4_2.addParticles
              L5_2 = A2_2.name
              L6_2 = "sonicBoom"
              L7_2 = 1
              L8_2 = false
              L9_2 = false
              L10_2 = true
              L4_2(L5_2, L6_2, L7_2, L8_2, L9_2, L10_2)
              L4_2 = gamelua
              L4_2 = L4_2.playAudio
              L5_2 = gamelua
              L5_2 = L5_2.getAudioName
              L6_2 = gamelua
              L6_2 = L6_2.blockTable
              L6_2 = L6_2.blocks
              L7_2 = A2_2.definition
              L6_2 = L6_2[L7_2]
              L6_2 = L6_2.specialSound
              L5_2 = L5_2(L6_2)
              L6_2 = 1
              L7_2 = false
              L4_2(L5_2, L6_2, L7_2)
            end
            L4_2 = gamelua
            L4_2 = L4_2.applyForce
            L5_2 = A2_2.name
            L6_2 = 0
            L7_2 = gamelua
            L7_2 = L7_2.objects
            L7_2 = L7_2.worldGravity
            L7_2 = -L7_2
            L8_2 = A2_2.mass
            L7_2 = L7_2 * L8_2
            L7_2 = L7_2 + 1
            L8_2 = A2_2.x
            L9_2 = A2_2.y
            L4_2(L5_2, L6_2, L7_2, L8_2, L9_2)
          end
          L4_2 = A2_2.pinkTimer
          if L4_2 < 0 then
            A2_2.pinkTimer = nil
            L4_2 = gamelua
            L4_2.birdSpecialtyAvailable = false
            L4_2 = gamelua
            L4_2 = L4_2.addParticles
            L5_2 = A2_2.name
            L6_2 = "bubblePopBig"
            L7_2 = 1
            L8_2 = false
            L9_2 = false
            L10_2 = true
            L4_2(L5_2, L6_2, L7_2, L8_2, L9_2, L10_2)
            L4_2 = gamelua
            L4_2 = L4_2.triggerPinkBirdSpecialty
            L5_2 = A2_2
            L6_2 = gamelua
            L6_2 = L6_2.getObjectDefinition
            L7_2 = A1_2
            L6_2 = L6_2(L7_2)
            L6_2 = L6_2.specialtyRadius
            L7_2 = {}
            L8_2 = "P_BIRDBUBBLE_1"
            L9_2 = "P_BIRDBUBBLE_2"
            L10_2 = "P_BIRDBUBBLE_3"
            L7_2[1] = L8_2
            L7_2[2] = L9_2
            L7_2[3] = L10_2
            L4_2(L5_2, L6_2, L7_2)
          end
      end
      else
        elseif L14_2 == "BUBBLES" then
          L15_2 = gamelua
          L15_2 = L15_2.flyingBird
          L15_2 = L15_2.inBubble
          if not L15_2 then
            L15_2 = gamelua
            L15_2 = L15_2.flyingBird
            L15_2 = L15_2.pinkTimer
            if L15_2 == nil then
              L15_2 = gamelua
              L15_2 = L15_2.flyingBird
              L15_2 = L15_2.collision
              if L15_2 ~= true then
                L15_2 = gamelua
                L15_2 = L15_2.objects
                L15_2 = L15_2.world
                L16_2 = gamelua
                L16_2 = L16_2.flyingBird
                L16_2 = L16_2.name
                L15_2 = L15_2[L16_2]
                L15_2.pinkTimer = 1.5
              end
            end
          end
gravityX = L0_1
L0_1 = 0
maxAngleVel = L0_1
L0_1 = 0.5
lifeTime = L0_1
