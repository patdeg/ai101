# Minecraft AI Chatbot: Groq Integration Guide

This guide details how to build a Java plugin for a PaperMC (Minecraft) server. This plugin will allow players to interact with an NPC, send their chat message to the Groq API, and receive a response in-game.

We will cover two examples:

1.  **Example 1:** A direct, no-prompt query.
2.  **Example 2:** A query that uses a system prompt for role-playing.

## Prerequisites

1.  **Java 17+ JDK:** You must have the Java 17 Development Kit (or newer) installed.
2.  **VS Code:** [Visual Studio Code](https://code.visualstudio.com/) with the [Extension Pack for Java](https://marketplace.visualstudio.com/items?itemName=vscjava.vscode-java-pack) installed.
3.  **PaperMC Server:** A working [PaperMC server](https://papermc.io/downloads).
4.  **Citizens Plugin:** The [Citizens plugin `.jar` file](https://www.google.com/search?q=%5Bhttps://www.spigotmc.org/resources/citizens.13811/%5D\(https://www.spigotmc.org/resources/citizens.13811/\)) in your server's `/plugins` folder.
5.  **Groq API Key:** A free API key from [Groq](https://console.groq.com/keys).

-----

## 1\. VS Code Project Setup (Maven)

1.  **Open VS Code:** Open an empty folder for your project.
2.  **Create Maven Project:** Open the command palette (`Ctrl+Shift+P`) and type `Maven: Create Maven Project`. Select `maven-archetype-quickstart` and follow the prompts.
3.  **Open `pom.xml`:** This file manages your project's libraries. Replace its contents with this:

### `pom.xml`

This file adds the Paper API (for Minecraft), the Citizens API (for NPCs), and Google's Gson (for JSON).

```xml
<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://maven.apache.org/POM/4.0.0"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
    <modelVersion>4.0.0</modelVersion>

    <groupId>com.ai101</groupId>
    <artifactId>GroqBot</artifactId>
    <version>1.0.0</version>
    <packaging>jar</packaging>

    <properties>
        <maven.compiler.source>17</maven.compiler.source>
        <maven.compiler.target>17</maven.compiler.target>
    </properties>

    <repositories>
        <repository>
            <id>papermc-repo</id>
            <url>https.repo.papermc.io/repository/maven-public/</url>
        </repository>
        <repository>
            <id>jitpack.io</id>
            <url>https.jitpack.io</url>
        </repository>
    </repositories>

    <dependencies>
        <dependency>
            <groupId>io.papermc.paper</groupId>
            <artifactId>paper-api</artifactId>
            <version>1.20.4-R0.1-SNAPSHOT</version>
            <scope>provided</scope>
        </dependency>
        <dependency>
            <groupId>com.github.CitizensDev</groupId>
            <artifactId>CitizensAPI</artifactId>
            <version>2.0.33-SNAPSHOT</version>
            <scope>provided</scope>
        </dependency>
        <dependency>
            <groupId>com.google.code.gson</groupId>
            <artifactId>gson</artifactId>
            <version>2.10.1</version>
            <scope>compile</scope>
        </dependency>
    </dependencies>
    
    <build>
        <finalName>${project.artifactId}</finalName>
    </build>
</project>
```

4.  **Delete `App.java`:** Maven creates a test file. Find `src/main/java/com/ai101/App.java` and delete it.
5.  **Create `src/main/resources`:** In the `src` folder, create a new folder named `resources`.

-----

## 2\. The Plugin Structure

Your plugin needs two key files to start.

### `plugin.yml` (in `src/main/resources`)

This file tells Minecraft what your plugin is and what it needs to run.

```yaml
name: GroqBot
version: 1.0.0
# Change this to your package structure
main: com.ai101.groqbot.GroqBotPlugin
api-version: 1.20
# This ensures Citizens loads before our plugin
depend: [Citizens]
```

### `GroqBotPlugin.java` (in `src/main/java/com/ai101/groqbot`)

Create a package (e.g., `com.ai101.groqbot`) and add this main class. It just registers our listener.

```java
package com.ai101.groqbot;

import org.bukkit.plugin.java.JavaPlugin;

public class GroqBotPlugin extends JavaPlugin {

    @Override
    public void onEnable() {
        // Register our event listener class
        getServer().getPluginManager().registerEvents(new GroqListener(this), this);
        getLogger().info("GroqBot plugin enabled!");
    }

    @Override
    public void onDisable() {
        getLogger().info("GroqBot plugin disabled.");
    }
}
```

-----

## 3\. The Core Logic (`GroqListener.java`)

This class does all the work. It listens for clicks, manages a simple "chat" state, and calls the Groq API.

```java
package com.ai101.groqbot;

import com.google.gson.Gson;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import net.citizensnpcs.api.CitizensAPI;
import net.citizensnpcs.api.event.RightClickNPCEvent;
import net.citizensnpcs.api.npc.NPC;
import org.bukkit.Bukkit;
import org.bukkit.entity.Player;
import org.bukkit.event.EventHandler;
import org.bukkit.event.Listener;
import org.bukkit.event.player.AsyncPlayerChatEvent;
import org.bukkit.plugin.java.JavaPlugin;

import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

public class GroqListener implements Listener {

    // --- CONFIGURATION ---
    // Get these IDs in-game with /npc select
    private final int EXAMPLE_1_NPC_ID = 1; // Direct query bot
    private final int EXAMPLE_2_NPC_ID = 2; // Role-play bot

    // Paste your Groq API Key here
    // !! For production, load this from a file, not hardcoded !!
    private final String GROQ_API_KEY = "gsk_YourKeyHere";
    // --- END CONFIGURATION ---

    // Stores which player is talking to which bot
    private final Map<UUID, Integer> playerChatState = new HashMap<>();
    
    private final JavaPlugin plugin;
    private final Gson gson = new Gson();
    private final HttpClient httpClient = HttpClient.newHttpClient();

    public GroqListener(JavaPlugin plugin) {
        this.plugin = plugin;
    }

    /**
     * Called when a player right-clicks ANY NPC.
     */
    @EventHandler
    public void onNpcRightClick(RightClickNPCEvent event) {
        int npcId = event.getNPC().getId();
        Player player = event.getClicker();

        if (npcId == EXAMPLE_1_NPC_ID) {
            // Put player in "chat mode" for Example 1
            playerChatState.put(player.getUniqueId(), 1);
            player.sendMessage("§a[DirectBot] §fWhat is your question?");
        } else if (npcId == EXAMPLE_2_NPC_ID) {
            // Put player in "chat mode" for Example 2
            playerChatState.put(player.getUniqueId(), 2);
            player.sendMessage("§e[Villager] §fWhat can I help you with, traveler?");
        }
    }

    /**
     * Called when a player types in chat.
     */
    @EventHandler
    public void onPlayerChat(AsyncPlayerChatEvent event) {
        Player player = event.getPlayer();
        UUID playerId = player.getUniqueId();

        // Check if this player is in our chat state map
        if (!playerChatState.containsKey(playerId)) {
            return;
        }

        // 1. Cancel the message from public chat
        event.setCancelled(true);

        String playerMessage = event.getMessage();
        int exampleType = playerChatState.remove(playerId); // Remove from state
        
        JsonArray messages = new JsonArray();

        // 2. Build the JSON payload based on the example
        if (exampleType == 1) {
            // Example 1: Direct question
            messages.add(createMessage("user", playerMessage));
        } else if (exampleType == 2) {
            // Example 2: System prompt + User question
            messages.add(createMessage("system", "You are a helpful Minecraft villager. Keep responses to 1-2 sentences."));
            messages.add(createMessage("user", playerMessage));
        }

        // 3. Run the API call ASYNC to not lag the server
        Bukkit.getScheduler().runTaskAsynchronously(plugin, () -> {
            try {
                String groqResponse = getGroqCompletion(messages);

                // 4. Send the response BACK to the main server thread
                Bukkit.getScheduler().runTask(plugin, () -> {
                    // Find the NPC they were talking to
                    int npcId = (exampleType == 1) ? EXAMPLE_1_NPC_ID : EXAMPLE_2_NPC_ID;
                    NPC npc = CitizensAPI.getNPCRegistry().getById(npcId);
                    if (npc != null) {
                        npc.chat(groqResponse); // Make the NPC "say" the response
                    }
                });

            } catch (Exception e) {
                Bukkit.getScheduler().runTask(plugin, () -> {
                    player.sendMessage("§c[GroqBot] §fAPI Error: " + e.getMessage());
                });
                plugin.getLogger().warning("Groq API Error: " + e.getMessage());
            }
        });
    }

    /**
     * Makes the actual HTTP call to the Groq API.
     * This method MUST be run on an async thread!
     */
    private String getGroqCompletion(JsonArray messages) throws Exception {
        
        // 1. Build the full JSON payload
        JsonObject payload = new JsonObject();
        payload.add("messages", messages);
        payload.addProperty("model", "llama3-8b-8192");
        String requestBody = gson.toJson(payload);

        // 2. Create the HTTP request
        HttpRequest request = HttpRequest.newBuilder()
                .uri(URI.create("https://api.groq.com/openai/v1/chat/completions"))
                .header("Authorization", "Bearer " + GROQ_API_KEY)
                .header("Content-Type", "application/json")
                .POST(HttpRequest.BodyPublishers.ofString(requestBody))
                .build();

        // 3. Send the request
        HttpResponse<String> response = httpClient.send(request, HttpResponse.BodyHandlers.ofString());

        // 4. Parse the JSON response
        JsonObject jsonResponse = gson.fromJson(response.body(), JsonObject.class);
        return jsonResponse.getAsJsonArray("choices")
                           .get(0).getAsJsonObject()
                           .getAsJsonObject("message")
                           .get("content").getAsString();
    }

    /**
     * Helper utility to create a JSON message object.
     */
    private JsonObject createMessage(String role, String content) {
        JsonObject message = new JsonObject();
        message.addProperty("role", role);
        message.addProperty("content", content);
        return message;
    }
}
```

-----

## 4\. Build and Run the Plugin

1.  **Build the `.jar`:**

      * Open the VS Code command palette (`Ctrl+Shift+P`).
      * Run `Maven: Execute Maven Command...`
      * Select `package` (or just type `mvn clean package` in the terminal).
      * This creates your plugin `.jar` file in the `target` folder (e.g., `GroqBot.jar`).

2.  **Install the Plugin:**

      * Copy your `GroqBot.jar` file into your PaperMC server's `/plugins` folder.
      * Make sure you also have the `Citizens.jar` file in that folder.

3.  **Start Your Server:** Run your PaperMC server.

4.  **Create Your NPCs (In-Game):**

      * Log into your server.
      * Type `/npc create "DirectBot"` to create the first bot.
      * Type `/npc create "Villager"` to create the second bot.

5.  **Get NPC IDs:**

      * Look at your "DirectBot" and type `/npc select`. The game will say `Selected NPC: 1` (or some ID).
      * Look at your "Villager" and type `/npc select`. Note its ID.
      * **This is important\!** You must update the `EXAMPLE_1_NPC_ID` and `EXAMPLE_2_NPC_ID` variables in `GroqListener.java` with the IDs you just got.

6.  **Re-Build and Restart:**

      * After saving your changes to `GroqListener.java`, re-build the plugin (`mvn clean package`).
      * Copy the new `.jar` file to your `/plugins` folder.
      * Restart your server.

You can now right-click your "DirectBot" to ask direct questions (Example 1) and your "Villager" to have a role-playing chat (Example 2).
