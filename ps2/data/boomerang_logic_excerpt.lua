-- Excerpt of gamelogic.lua (boomerang specialty: activation and per-frame update), recovered from the supplied Classic APK; read only.
  L4_2 = birdSpecialty
  if L4_2 == "BOOMERANG" then
    L4_2 = settingsWrapper
    L5_2 = L4_2
    L4_2 = L4_2.setExtraTutorialShown
    L6_2 = L2_2.sprite
    L4_2(L5_2, L6_2)
    A0_2.boomerangActive = true
    L4_2 = L2_2.facebookBoomerangMode
    if L4_2 then
      L4_2 = A0_2.xVel
      L4_2 = -1.5 * L4_2
      A0_2.boomerangTargetXVel = L4_2
    else
      L4_2 = A0_2.xVel
      if L4_2 ~= 0 then
        L4_2 = _G
        L4_2 = L4_2.math
        L4_2 = L4_2.min
        L5_2 = _G
        L5_2 = L5_2.math
        L5_2 = L5_2.abs
        L6_2 = A0_2.yVel
        L7_2 = A0_2.xVel
        L6_2 = L6_2 / L7_2
        L5_2 = L5_2(L6_2)
        L6_2 = 2
        L4_2 = L4_2(L5_2, L6_2)
        L4_2 = 2 - L4_2
        yForceCoeff = L4_2
      else
        L4_2 = 0
        yForceCoeff = L4_2
      end
      L4_2 = L4_1
      L5_2 = L4_2
      L4_2 = L4_2.getSlingshotPower
      L4_2 = L4_2(L5_2)
      L5_2 = A0_2.xVel
      L6_2 = physicsScale
      L5_2 = L5_2 * L6_2
      L6_2 = A0_2.mass
      L5_2 = L5_2 * L6_2
      L6_2 = blockTable
      L6_2 = L6_2.blocks
      L7_2 = A0_2.definition
      L6_2 = L6_2[L7_2]
      L6_2 = L6_2.boomerangHorizontalForce
      L5_2 = L5_2 * L6_2
      L5_2 = L5_2 * L4_2
      A0_2.boomerangXForce = L5_2
      L5_2 = yForceCoeff
      L6_2 = physicsScale
      L5_2 = L5_2 * L6_2
      L6_2 = A0_2.mass
      L5_2 = L5_2 * L6_2
      L6_2 = blockTable
      L6_2 = L6_2.blocks
      L7_2 = A0_2.definition
      L6_2 = L6_2[L7_2]
      L6_2 = L6_2.boomerangVerticalForce
      L5_2 = L5_2 * L6_2
      L5_2 = L5_2 * L4_2
      A0_2.boomerangYForce = L5_2
      L5_2 = blockTable
      L5_2 = L5_2.blocks
      L6_2 = A0_2.definition
      L5_2 = L5_2[L6_2]
      L5_2 = L5_2.boomerangMaxHorizontalSpeed
      L5_2 = -L5_2
      L5_2 = L5_2 * L4_2
      A0_2.boomerangMinXVel = L5_2
      L5_2 = blockTable
      L5_2 = L5_2.blocks
      L6_2 = A0_2.definition
      L5_2 = L5_2[L6_2]
      L5_2 = L5_2.boomerangMaxHorizontalSpeed
      L5_2 = L5_2 * L4_2
      A0_2.boomerangMaxXVel = L5_2
    end
    L4_2 = objects
    L4_2 = L4_2.world
-- per-frame update
function L24_1(A0_2)
  local L1_2, L2_2, L3_2, L4_2, L5_2, L6_2, L7_2, L8_2, L9_2, L10_2, L11_2, L12_2
  L1_2 = _G
  L1_2 = L1_2.pairs
  L2_2 = birds
  L1_2, L2_2, L3_2 = L1_2(L2_2)
  for L4_2, L5_2 in L1_2, L2_2, L3_2 do
    L6_2 = objects
    L6_2 = L6_2.world
    L6_2 = L6_2[L4_2]
    if L6_2 then
      L6_2 = getObjectDefinition
      L7_2 = L5_2.name
      L6_2 = L6_2(L7_2)
      L7_2 = L5_2.shot
      if L7_2 == true then
        L7_2 = L6_2.specialty
        if L7_2 == "BOOMERANG" then
          L7_2 = L5_2.boomerangActive
          if L7_2 == true then
            L7_2 = L5_2.prevAngle
            if L7_2 ~= nil then
              L7_2 = L5_2.prevAngle
              if L7_2 < -1 then
                L7_2 = L5_2.angle
                if -1 <= L7_2 then
                  L7_2 = L5_2.x
                  L8_2 = levelLeftEdge
                  if L7_2 < L8_2 then
                    L7_2 = _G
                    L7_2 = L7_2.math
                    L7_2 = L7_2.max
                    L8_2 = levelLeftEdge
                    L9_2 = L5_2.x
                    L8_2 = L8_2 - L9_2
                    L8_2 = L8_2 / 100
                    L8_2 = 1 - L8_2
                    L9_2 = 0
                    L7_2 = L7_2(L8_2, L9_2)
                    vol = L7_2
                  else
                    L7_2 = L5_2.x
                    L8_2 = levelRightEdge
                    if L7_2 > L8_2 then
                      L7_2 = _G
                      L7_2 = L7_2.math
                      L7_2 = L7_2.max
                      L8_2 = L5_2.x
                      L9_2 = levelRightEdge
                      L8_2 = L8_2 - L9_2
                      L8_2 = L8_2 / 100
                      L8_2 = 1 - L8_2
                      L9_2 = 0
                      L7_2 = L7_2(L8_2, L9_2)
                      vol = L7_2
                    else
                      L7_2 = 1
                      vol = L7_2
                    end
                  end
                  L7_2 = playAudio
                  L8_2 = "boomerang_swish"
                  L9_2 = vol
                  L10_2 = false
                  L7_2(L8_2, L9_2, L10_2)
                end
              end
            end
            L7_2 = L5_2.angle
            L5_2.prevAngle = L7_2
            L7_2 = L6_2.facebookBoomerangMode
            if L7_2 then
              L7_2 = L5_2.boomerangTargetXVel
              if L7_2 ~= 0 then
                L7_2 = L5_2.boomerangTargetXVel
                L8_2 = L5_2.xVel
                if L7_2 < L8_2 then
                  L7_2 = setVelocity
                  L8_2 = L5_2.name
                  L9_2 = L5_2.xVel
                  L10_2 = A0_2 * 1000
                  L10_2 = L10_2 / 10
                  L9_2 = L9_2 - L10_2
                  L10_2 = L5_2.yVel
                  L7_2(L8_2, L9_2, L10_2)
                  L7_2 = L5_2.boomerangTargetXVel
                  L8_2 = L5_2.xVel
                  if L7_2 >= L8_2 then
                    L5_2.boomerangTargetXVel = 0
                  end
                else
                  L7_2 = L5_2.boomerangTargetXVel
                  L8_2 = L5_2.xVel
                  if L7_2 > L8_2 then
                    L7_2 = setVelocity
                    L8_2 = L5_2.name
                    L9_2 = L5_2.xVel
                    L10_2 = A0_2 * 1000
                    L10_2 = L10_2 / 10
                    L9_2 = L9_2 + L10_2
                    L10_2 = L5_2.yVel
                    L7_2(L8_2, L9_2, L10_2)
                    L7_2 = L5_2.boomerangTargetXVel
                    L8_2 = L5_2.xVel
                    if L7_2 <= L8_2 then
                      L5_2.boomerangTargetXVel = 0
                    end
                  end
                end
              end
            else
              L7_2 = L5_2.xVel
              L8_2 = L5_2.boomerangMinXVel
              if L7_2 >= L8_2 then
                L7_2 = L5_2.xVel
                L8_2 = L5_2.boomerangMaxXVel
                if L7_2 <= L8_2 then
                  L7_2 = applyForce
                  L8_2 = L5_2.name
                  L9_2 = L5_2.boomerangXForce
                  L10_2 = 0
                  L11_2 = L5_2.x
                  L12_2 = L5_2.y
                  L7_2(L8_2, L9_2, L10_2, L11_2, L12_2)
                end
              end
              L7_2 = L5_2.yVel
              if 5 < L7_2 then
                L7_2 = applyForce
                L8_2 = L5_2.name
                L9_2 = 0
                L10_2 = L5_2.boomerangYForce
                L11_2 = L5_2.x
                L12_2 = L5_2.y
                L7_2(L8_2, L9_2, L10_2, L11_2, L12_2)
              end
            end
            L7_2 = L5_2.angularVelocity
            L8_2 = A0_2 * 20
            L7_2 = L7_2 + L8_2
            L5_2.angularVelocity = L7_2
            L7_2 = L5_2.angularVelocity
            if 20 < L7_2 then
              L5_2.angularVelocity = 20
            end
            L7_2 = setAngularVelocity
            L8_2 = L5_2.name
            L9_2 = L5_2.angularVelocity
            L7_2(L8_2, L9_2)
          else
            L7_2 = flyingBird
            if L5_2 == L7_2 then
              L7_2 = birdSpecialtyAvailable
              if L7_2 then
                L7_2 = blockTable
                L7_2 = L7_2.blocks
                L8_2 = L5_2.definition
                L7_2 = L7_2[L8_2]
                L7_2 = L7_2.flyVerticalAcceleration
                if L7_2 ~= nil then
                  L7_2 = applyForce
                  L8_2 = L5_2.name
                  L9_2 = 0
                  L10_2 = blockTable
                  L10_2 = L10_2.blocks
                  L11_2 = L5_2.definition
                  L10_2 = L10_2[L11_2]
                  L10_2 = L10_2.flyVerticalAcceleration
                  L11_2 = L5_2.mass
                  L10_2 = L10_2 * L11_2
                  L11_2 = L5_2.x
                  L12_2 = L5_2.y
                  L7_2(L8_2, L9_2, L10_2, L11_2, L12_2)
                else
                  L7_2 = applyForce
                  L8_2 = L5_2.name
                  L9_2 = 0
                  L10_2 = blockTable
                  L10_2 = L10_2.blocks
                  L11_2 = L5_2.definition
                  L10_2 = L10_2[L11_2]
                  L10_2 = L10_2.flyVerticalForce
                  L11_2 = L5_2.x
                  L12_2 = L5_2.y
                  L7_2(L8_2, L9_2, L10_2, L11_2, L12_2)
                end
                L7_2 = L5_2.angularVelocity
                if L7_2 == nil then
                  L5_2.angularVelocity = 0
                end
                L7_2 = L5_2.angularVelocity
                L8_2 = A0_2 * 15
                L7_2 = L7_2 + L8_2
                L5_2.angularVelocity = L7_2
                L7_2 = L5_2.angularVelocity
                if 6.28 < L7_2 then
                  L5_2.angularVelocity = 6.28
                end
                L7_2 = setAngularVelocity
                L8_2 = L5_2.name
                L9_2 = L5_2.angularVelocity
                L7_2(L8_2, L9_2)
              end
            end
          end
        end
