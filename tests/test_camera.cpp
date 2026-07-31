#include "catch_amalgamated.hpp"
#include "core/camera/camera.hpp"

TEST_CASE("Camera front direction follows yaw and pitch") {
    Camera cam;

    cam.setYaw(-90.0f);
    cam.setPitch(0.0f);
    glm::vec3 front = cam.getFront();
    REQUIRE(front.x == Catch::Approx(0.0f).margin(1e-5f));
    REQUIRE(front.y == Catch::Approx(0.0f).margin(1e-5f));
    REQUIRE(front.z == Catch::Approx(-1.0f));

    cam.setYaw(0.0f);
    cam.setPitch(0.0f);
    front = cam.getFront();
    REQUIRE(front.x == Catch::Approx(1.0f));
    REQUIRE(front.y == Catch::Approx(0.0f).margin(1e-5f));
    REQUIRE(front.z == Catch::Approx(0.0f).margin(1e-5f));

    cam.setYaw(-90.0f);
    cam.setPitch(90.0f);
    front = cam.getFront();
    REQUIRE(front.x == Catch::Approx(0.0f).margin(1e-5f));
    REQUIRE(front.y == Catch::Approx(1.0f));
    REQUIRE(front.z == Catch::Approx(0.0f).margin(1e-5f));
}

TEST_CASE("Camera view matrix places the camera at the origin") {
    Camera cam;
    cam.setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    cam.setYaw(-90.0f);
    cam.setPitch(0.0f);

    glm::mat4 view = cam.getViewMatrix();
    glm::vec4 atOrigin = view * glm::vec4(cam.getPosition(), 1.0f);

    REQUIRE(atOrigin.x == Catch::Approx(0.0f).margin(1e-5f));
    REQUIRE(atOrigin.y == Catch::Approx(0.0f).margin(1e-5f));
    REQUIRE(atOrigin.z == Catch::Approx(0.0f).margin(1e-5f));
}

TEST_CASE("Camera view matrix translates by negative position") {
    Camera cam;
    cam.setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    cam.setYaw(-90.0f);
    cam.setPitch(0.0f);

    glm::mat4 view = cam.getViewMatrix();

    REQUIRE(view[3][0] == Catch::Approx(0.0f).margin(1e-5f));
    REQUIRE(view[3][1] == Catch::Approx(0.0f).margin(1e-5f));
    REQUIRE(view[3][2] == Catch::Approx(-3.0f));
}

TEST_CASE("Camera view matrix rotates yaw so the look direction maps to -Z") {
    Camera cam;
    cam.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    cam.setYaw(0.0f);
    cam.setPitch(0.0f);

    glm::mat4 view = cam.getViewMatrix();
    glm::vec4 forward = view * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);

    REQUIRE(forward.x == Catch::Approx(0.0f).margin(1e-5f));
    REQUIRE(forward.y == Catch::Approx(0.0f).margin(1e-5f));
    REQUIRE(forward.z == Catch::Approx(-1.0f));
}

TEST_CASE("Camera projection matrix scales with aspect ratio") {
    Camera cam;
    float expectedF = static_cast<float>(1.0 / tan(glm::radians(45.0) / 2.0));

    glm::mat4 proj1 = cam.getProjectionMatrix(1.0f);
    REQUIRE(proj1[0][0] == Catch::Approx(expectedF));
    REQUIRE(proj1[1][1] == Catch::Approx(expectedF));

    glm::mat4 proj2 = cam.getProjectionMatrix(2.0f);
    REQUIRE(proj2[0][0] == Catch::Approx(expectedF / 2.0f));
    REQUIRE(proj2[1][1] == Catch::Approx(expectedF));
}

TEST_CASE("Camera projection maps near and far planes to NDC -1 and +1") {
    Camera cam;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    cam.setClippingPlanes(nearPlane, farPlane);

    glm::mat4 proj = cam.getProjectionMatrix(1.0f);

    glm::vec4 nearClip = proj * glm::vec4(0.0f, 0.0f, -nearPlane, 1.0f);
    REQUIRE(nearClip.z / nearClip.w == Catch::Approx(-1.0f));

    glm::vec4 farClip = proj * glm::vec4(0.0f, 0.0f, -farPlane, 1.0f);
    REQUIRE(farClip.z / farClip.w == Catch::Approx(1.0f));
}

TEST_CASE("Camera setters and getters round-trip values") {
    Camera cam;

    cam.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
    cam.setYaw(45.0f);
    cam.setPitch(-15.0f);
    cam.setFov(60.0f);
    cam.setClippingPlanes(0.5f, 500.0f);

    REQUIRE(cam.getPosition() == glm::vec3(1.0f, 2.0f, 3.0f));
    REQUIRE(cam.getYaw() == Catch::Approx(45.0f));
    REQUIRE(cam.getPitch() == Catch::Approx(-15.0f));
    REQUIRE(cam.getFov() == Catch::Approx(60.0f));
    REQUIRE(cam.getNearPlane() == Catch::Approx(0.5f));
    REQUIRE(cam.getFarPlane() == Catch::Approx(500.0f));
}
